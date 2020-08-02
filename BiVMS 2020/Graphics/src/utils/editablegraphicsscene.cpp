#include <type_traits>
#include <QtGui>
#include <QVector>
#include <QtCore/qmath.h>
#include "utils\editablegraphicsscene.h"
#include "primitives\rect.h"
#include "primitives\polygon.h"
#include "primitives\ellips.h"
#include "operations\polygonmerger.h"
#include "operations\polygonclipper.h"
#include "utils\handcursorevents.h"
#include "operations\rectclipper.h"
#include "operations\polygonclipper2.h"
#include "operations\ellipsclipper.h"
#include "operations\shapedeleter.h"
#include "utils\mousetracker.h"

#define PI 3.14159265358979323846264338327950288419717

EditableGraphicsScene::EditableGraphicsScene(QObject* parent)
	: WheelScalableGraphicsScene(parent)
{
	m_is_addpoint = false;
	m_isdrawing = false;
	m_is_merge_poly = true;
	m_temp_shape = nullptr;
	m_triangle_shape = nullptr;
	m_handcursor = false;
	m_selectedregions = 0;
	m_selshapekg = false;

	m_is_cursor_info_visible = false;
	m_is_seed_selected = false;
	m_is_clipping = false;
}
EditableGraphicsScene::~EditableGraphicsScene() {}

#pragma region Mouse Events
void EditableGraphicsScene::mousePressEvent(QGrSME *e)
{
	m_eventpos = e->scenePos();
	if (!m_showImageItem)
		return;
	if (!m_showImageItem->contains(m_eventpos))
		return;
	emit change_cursor_shape(Qt::CrossCursor);
	m_last_pressedpos = CorrectPointInSceneRect(e->scenePos());

	if (e->button() == Qt::RightButton)
	{
		m_drawstatus = _MyStatus::READY;
		if (m_isdrawing) emit right_button_drawing();
		switch (m_drawstatus)
		{
		case _MyStatus::READY: break;

		case _MyStatus::DRAW_RECT: {
			if (m_isdrawing) {
				ShapeEvent* pCmd = nullptr;
				pCmd = new RectPainter(this,
					QPointF(0, 0), QPointF(0, 0));
				pCmd->merge();
				m_isdrawing = false;
				emit complete();
			}
			break;
		}
		case _MyStatus::DRAW_POLY: {
			ShapeEvent* pCmd = nullptr;
			pCmd = new PolygonPainter(this, QPointF(0, 0));
			pCmd->merge();
			m_isdrawing = false;
			emit complete();
			break;
		}
		case _MyStatus::DRAW_CIRC: {
			EllipsPainter* pCmd = new EllipsPainter(
				this, m_last_pressedpos);
			pCmd->right_click();
			m_handcursor = false;
			m_isdrawing = false;
			emit complete();
			break;
		}
		case _MyStatus::CLIP_RECT: {
			if (m_is_clipping) {
				ShapeClippingEvents* pCmd = nullptr;
				pCmd = new RectClipper(this,
					m_last_pressedpos, m_last_pressedpos);
				pCmd->clip();
				m_isdrawing = false;
				m_is_clipping = false;
				emit complete();
			}
			break;
		}
		case _MyStatus::CLIP_POLY: {
			if (m_is_clipping) {
				ShapeClippingEvents* pCmd = nullptr;
				pCmd = new PolygonClipper(this, QPointF(0, 0));
				pCmd->clip();
				m_isdrawing = false;
				m_is_clipping = false;
				emit complete();
			}
			break;
		}
		case _MyStatus::CLIP_CIRC: {
			EllipsClipper* pCmd = new EllipsClipper(
				this, m_last_pressedpos);
			pCmd->right_click();
			m_isdrawing = false;
			emit complete();
			break;
		}
		case _MyStatus::DEL_SHAPE: {
			DeleteAreaCommand *pCmd = nullptr;
			pCmd = new DeleteAreaCommand(
				this, m_last_pressedpos);
			pCmd->exec();
			break; }
		}
	}

	if (e->button() == Qt::LeftButton)
	{
		switch (m_drawstatus)
		{
		case _MyStatus::READY:
			if (!m_isdrawing) {
				HandCursorEvent* pCmd = nullptr;
				pCmd = new HandCursorEvent(this, m_eventpos);
				pCmd->press();
				m_isdrawing = false;
				emit complete();
			}
			break;
		case _MyStatus::DRAW_RECT:
			if (!m_isdrawing)   // start drawing rectangular
			{
				ShapeEvent* pCmd = nullptr;
				pCmd = new RectPainter(this,
					m_last_pressedpos,
					m_last_pressedpos);
				pCmd->exec();
				m_handcursor = false;
				m_isdrawing = true;
			}
			else  // end drawing rectangular
			{
				ShapeEvent* pCmd = nullptr;
				pCmd = new RectPainter(this,
					QPointF(0, 0), QPointF(0, 0));
				pCmd->merge();
				m_handcursor = true;
				m_isdrawing = false;
				emit complete();
			}
			return;
		case _MyStatus::DRAW_POLY:
			if (!m_isdrawing) // start drawing polygon
			{
				ShapeEvent* pCmd = nullptr;
				pCmd = new PolygonPainter(
					this, m_last_pressedpos);
				pCmd->exec();
				m_handcursor = false;
				m_isdrawing = true;
			}
			else // continue drawing polygon
			{
				PolygonPainter* pCmd = nullptr;
				pCmd = new PolygonPainter(
					this, m_last_pressedpos);
				pCmd->next();
			}
			break;
		case _MyStatus::DRAW_CIRC:
			if (!m_isdrawing)  // start drawing circle
			{
				ShapeEvent* pCmd = nullptr;
				pCmd = new EllipsPainter(
					this, m_last_pressedpos);
				pCmd->exec();
				m_handcursor = true;
				m_isdrawing = true;
			}
			else {
				ShapeEvent* pCmd = nullptr;
				pCmd = new EllipsPainter(
					this, m_last_pressedpos);
				pCmd->merge();
				if (!m_isdrawing)  // drawing complete
					emit complete();
			}
			break;
		case _MyStatus::DEL_SHAPE: // delete area node or area
			if (!DeleteSeed(m_eventpos) &&
				!DeleteNode(m_last_pressedpos) &&
				!delete_point_item<BGpointItem>(m_eventpos))// if we delete one node or a seed, we'll not delete the area
			{
				DeleteAreaCommand* pCmd = nullptr; 	// delete area
				pCmd = new DeleteAreaCommand(
					this, m_last_pressedpos);
				pCmd->exec();
			}
			break;
		case _MyStatus::CLIP_RECT:
			if (!m_shapes.isEmpty()) {
				if (!m_isdrawing) // start to cut rectangle
				{
					ShapeClippingEvents* pCmd = nullptr;
					pCmd = new RectClipper(this,
						m_last_pressedpos,
						m_last_pressedpos);
					pCmd->exec();
					m_handcursor = false;
					m_isdrawing = true;
					m_is_clipping = true;
				}
				else  // end cutting
				{
					ShapeClippingEvents* pCmd = nullptr;
					pCmd = new RectClipper(this,
						m_last_pressedpos,
						m_last_pressedpos);
					pCmd->clip();
					m_handcursor = true;
					m_isdrawing = false;
					m_is_clipping = false;
					emit complete();
				}
			}
			return;
		case _MyStatus::CLIP_POLY:
			if (!m_isdrawing) // start to cut polygon 
			{
				ShapeClippingEvents* pCmd = nullptr;
				pCmd = new PolygonClipper(
					this, m_last_pressedpos);
				pCmd->exec();
				m_handcursor = false;
				m_isdrawing = true;
				m_is_clipping = true;
			}
			else // continue to cut polygon 
			{
				PolygonClipper* pCmd = nullptr;
				pCmd = new PolygonClipper(
					this, m_last_pressedpos);
				pCmd->next();
			}
			break;
		case _MyStatus::CLIP_CIRC:
			if (!m_isdrawing) // start to cut circle, the first point
			{
				ShapeClippingEvents* pCmd = nullptr;
				pCmd = new EllipsClipper(
					this, m_last_pressedpos);
				pCmd->exec();
				m_handcursor = false;
				m_isdrawing = true;
				return;
			}
			else // continue to cut the circle
			{
				ShapeClippingEvents* pCmd = nullptr;
				pCmd = new EllipsClipper(
					this, m_last_pressedpos);
				pCmd->clip();
				if (!m_isdrawing)
					emit complete();
			}
			break;
		case _MyStatus::DRAW_POINT:  // add new seed point
		{
			SeedItem* seed = new SeedItem;
			addItem(seed);
			seed->setPos(m_last_pressedpos);
			m_seeds << seed;

			QGraphicsSimpleTextItem *number = new QGraphicsSimpleTextItem;
			number->setBrush(Qt::red);
			number->setText(QString::number(m_seeds.size()));
			number->setFont(QFont("华文琥珀", 15));
			number->setFlag(QGraphicsItem::ItemIsMovable, false);
			number->setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
			number->setZValue(2.0);
			addItem(number);
			number->setPos(m_last_pressedpos);
			m_numbers << number;

			m_isdrawing = false;
			emit selectedIndex(m_seeds.size() - 1);
			emit complete();
			
		}
		break;
		case _MyStatus::DRAW_CALIBLINE:
		{
			if (m_endpts.size() < 2) {
				SeedItem* seed = new SeedItem;
				addItem(seed);
				seed->setPos(m_last_pressedpos);
				m_endpts << seed;
				m_isdrawing = false;
				emit complete();
			}
			else {
				m_drawstatus = _MyStatus::READY;
				emit complete();
			}

		}
		break;
		case _MyStatus::DRAW_BGPOINT:  // add new seed point
		{
			BGpointItem* seed = new BGpointItem;
			addItem(seed);
			seed->setPos(m_last_pressedpos);
			m_bgpoints << seed;
			m_isdrawing = false;
			emit complete();
		}
		break;
		case _MyStatus::DEL_NODE: // delete area node or seed point
		{
			DeleteSeed(m_eventpos);  // delete seed
			DeleteNode(m_last_pressedpos);  // delete node
			delete_point_item<BGpointItem>(m_eventpos);
		}
		break;
		case  _MyStatus::AUTO_SEED: // auto find aoi area
		{
			emit SignalSSSIGPointPicked(
				QPointF(m_last_pressedpos));
			m_drawstatus = m_last_drawstatus;
			emit complete();
		}
		break;
		}
	}
	QGraphicsScene::mousePressEvent(e);
}
void EditableGraphicsScene::mouseMoveEvent(QGrSME *e)
{
	if (m_isdrawing)
	{
		m_eventpos = e->scenePos();
		if (!m_showImageItem->contains(m_eventpos))
			return;
	}
	emit change_cursor_shape(Qt::CrossCursor);
	QPointF mouse_pos = CorrectPointInSceneRect(e->scenePos());

	if (!m_isdrawing && m_drawstatus == _MyStatus::READY) {
		HandCursorEvent* pCmd = new  HandCursorEvent(this, mouse_pos);
		pCmd->move(); // display the node as red if mouse on this node

		/* if a seed point is selected, move the seed point */
		if (m_is_seed_selected && m_translate_seed != nullptr) {
			m_eventpos = e->scenePos();
			if (!m_showImageItem->contains(m_eventpos))
				return;
			QPointF pos = CorrectPointInSceneRect(m_last_seed_pos +
				e->scenePos() - m_last_pressedpos);
			m_translate_seed->setPos(pos);   // the the seed position
			for (int i = 0; i < m_seeds.size(); ++i)
			{
				m_numbers.at(i)->setPos(m_seeds.at(i)->pos());
			}
			MoveCursorLabel(mouse_pos);  // move the coordinate label
			return;
		}
		/* if a node is selected, move the node */
		for (int area_order = 0; area_order < m_shapes.length();
			area_order++)
		{
			if (m_shapes.value(area_order)->item_select) {
				m_selectedregions = area_order;
				m_selshapekg = true;
				setAreaToNode(mouse_pos);
				AddInitSet();
				AddRegionMove(m_shapes.value(m_selectedregions));
				MoveCursorLabel(mouse_pos);
			}
		}
		emit complete();
	}

	if (m_isdrawing) {
		switch (m_drawstatus)
		{
		case _MyStatus::READY: {
			m_isdrawing = false;
			break;
		}
		case _MyStatus::DRAW_RECT: {
			ShapeEvent* pCmd = nullptr;
			pCmd = new RectPainter(this,
				m_last_pressedpos, mouse_pos);
			pCmd->move();
		}
								   break;
		case _MyStatus::DRAW_POLY: {
			ShapeEvent* pCmd = nullptr;
			pCmd = new PolygonPainter(this, mouse_pos);
			pCmd->move();
		}
								   break;
		case _MyStatus::DRAW_CIRC: {
			ShapeEvent* pCmd = nullptr;
			pCmd = new EllipsPainter(this, mouse_pos);
			pCmd->move();
		}
								   break;
		case _MyStatus::CLIP_RECT: {
			ShapeClippingEvents* pCmd = nullptr;
			pCmd = new RectClipper(
				this, m_last_pressedpos, mouse_pos);
			pCmd->move();
		}
								   break;
		case _MyStatus::CLIP_POLY: {
			ShapeClippingEvents* pCmd = nullptr;
			pCmd = new PolygonClipper(this, mouse_pos);
			pCmd->move();
		}
								   break;
		case _MyStatus::CLIP_CIRC: {
			ShapeClippingEvents* pCmd = nullptr;
			pCmd = new EllipsClipper(this, mouse_pos);
			pCmd->move();
		}
								   break;
		}
	}
	if (m_mouse_delegate != nullptr)
	{
		m_mouse_delegate(e);
	}
	QGraphicsScene::mouseMoveEvent(e);
}
void EditableGraphicsScene::mouseReleaseEvent(QGrSME* e)
{
	emit change_cursor_shape(Qt::CrossCursor);
	if (m_is_seed_selected) {
		DeleteCursorLabel();
		m_is_seed_selected = false;
	}
	for (int area_order = 0; area_order < m_shapes.length(); area_order++)
	{
		if ((m_shapes.value(area_order)->item_select)
			&& (m_drawstatus == _MyStatus::READY)) {
			m_selectedregions = area_order;
			setAreaToNode(e->scenePos());
			AddInitSet();
			AddRegionPress(m_shapes.value(m_selectedregions));
			DeleteCursorLabel();
		}
	}
	QGraphicsScene::mouseReleaseEvent(e);
}
void EditableGraphicsScene::mouseDoubleClickEvent(QGrSME* e)
{
	m_eventpos = e->scenePos();
	if (!m_showImageItem->contains(m_eventpos))
		return;
	if ((m_translate_seed =
		cursor_captured_seed(e->scenePos())) != nullptr)
	{
		m_is_seed_selected = true;
		m_last_seed_pos = QPointF(m_translate_seed->scenePos());
		CreateCursorLabel();
	}
	e->ignore();
}
void EditableGraphicsScene::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Escape:
		if (event->modifiers() & Qt::ControlModifier)
		{
			m_isdrawing = false;
			emit complete();
		}
		break;
	default:
		QGraphicsScene::keyPressEvent(event);
	}
}
#pragma endregion

#pragma region Public methods

/* for a region, this method will change the region to a list of ShapeItem* if a node is deleted */
QVector<ShapeItem*> EditableGraphicsScene::
AreaDeleteNodeToAreas(ShapeItem* area, NodeItem* nodeSelected)
{
	QPointF pos_refer;   // pos_refer is the reference point
	if (nodeSelected == area->node_list.value(0))
		pos_refer = area->node_list.value(1)->scenePos();
	else
		pos_refer = area->node_list.value(0)->scenePos();

	QVector<ShapeItem*> areas;
	ShapeItem* areasListItem = new ShapeItem();
	QPolygonF areasListItemPolygon;

	for (auto nodeIterator = area->node_list.begin();
		nodeIterator != area->node_list.end(); nodeIterator++)
	{
		float x = (*nodeIterator)->x();
		float y = (*nodeIterator)->y();

		if (x != -1.0 || y != -1.0) {
			if (*nodeIterator != nodeSelected)
			{
				NodeItem* node = new NodeItem(areasListItem);
				node->setPos(QPointF(x, y));
				areasListItem->node_list << node;
				areasListItemPolygon << QPointF(x, y) - pos_refer;
			}
		}
		else if (x == -1.0 && y == -1.0)  // if we meet the partition point (-1,-1)
		{
			if (areasListItem->node_list.length() >= 3)
			{
				areasListItem->setPos(pos_refer);
				areasListItem->setPolygon(areasListItemPolygon);
				areas << areasListItem;
			}
			areasListItem = new ShapeItem();
			areasListItemPolygon.clear();
		}
	}

	if (areasListItem->node_list.length() >= 3)
	{
		areasListItem->setPos(pos_refer);
		areasListItem->setPolygon(areasListItemPolygon);
		areas << areasListItem;
	}

	return areas;
}
/* return true if a seed is deleted, vice versa */
bool EditableGraphicsScene::DeleteSeed(QPointF point)
{
	bool isDeleted = false;

	for (int i = 0; i < m_seeds.length(); i++) {
		QPainterPath *pPainterPath = &m_seeds.value(i)->m_image_path;
		QTransform trans = this->views().value(0)->transform();
		double scalfac_previous = trans.m11();
		QPointF _point;
		_point.setX(scalfac_previous*(point.x() -
			m_seeds.value(i)->x()));
		_point.setY(scalfac_previous*(point.y() -
			m_seeds.value(i)->y()));

		if (pPainterPath->contains(_point)) {
			removeItem(m_seeds.value(i));
			m_seeds.removeAt(i);
			removeItem(m_numbers.value(i));
			m_numbers.removeAt(i);
			for (int i = 0; i < m_seeds.size(); ++i)
			{
				m_numbers.at(i)->setText(QString::number(i+1));
			}
			m_isdrawing = false;
			isDeleted = true;
			emit selectedIndex(i);
			emit complete();
			
		}
	}
	return isDeleted;
}

template<typename _Item>
bool EditableGraphicsScene::delete_point_item(QPointF point)
{
	if (std::is_same_v<_Item, SeedItem>) {
		return DeleteSeed(point);
	}
	else if (std::is_same_v<_Item, BGpointItem>) {
		bool isDeleted = false;
		for (int i = 0; i < m_bgpoints.length(); i++) {
			QPainterPath *pPainterPath = &m_bgpoints.value(i)->shape();
			QTransform trans = this->views().value(0)->transform();
			double scalfac_previous = trans.m11();
			QPointF _point;
			_point.setX(scalfac_previous*(point.x() -
				m_bgpoints.value(i)->x()));
			_point.setY(scalfac_previous*(point.y() -
				m_bgpoints.value(i)->y()));

			if (pPainterPath->contains(_point)) {
				removeItem(m_bgpoints.value(i));
				m_bgpoints.removeAt(i);
				m_isdrawing = false;
				isDeleted = true;
				emit complete();
			}
		}
		return isDeleted;
	}
	else if (std::is_same_v<_Item, void>) {
		bool isdeleted = false;
		if (!m_endpts.empty())
		{
			removeItem(m_endpts.front());
			removeItem(m_endpts.back());
			m_endpts.clear();
			m_isdrawing = false;
			isdeleted = true;
			emit complete();
		}
	}
}

template bool EditableGraphicsScene::delete_point_item<void>(QPointF point);
template bool EditableGraphicsScene::delete_point_item<SeedItem>(QPointF point);
//template<> bool EditableGraphicsScene::delete_point_item<BGpointItem>(QPointF point);

bool EditableGraphicsScene::DeleteNode(QPointF point)
{
	bool isDeleted = false;
	foreach(ShapeItem * area, m_shapes)
	{
		if (area->item_select) {
			int count = area->node_list.length();
			double scalfac = this->views().first()->matrix().m11(); // zoom factor of the view
			foreach(NodeItem * node, area->node_list)
			{
				if (node->boundingRect().contains(node->mapFromScene(point) * scalfac))  // if point in node
				{
					if (count > 3) {
						QVector<ShapeItem*> areas = AreaDeleteNodeToAreas(area, node);
						ShapeItem* mainArea = areas.first();

						QList<QPointF> mainRegionPointsList;
						foreach(NodeItem * node, mainArea->node_list)
							mainRegionPointsList << QPointF(node->scenePos());

						QList<QPointF> clipRegionPointsList;
						for (int j = 1; j < areas.count(); j++) {
							if (AreaContainArea(mainArea, areas[j])) {	 // if main area contain the clipping area, we add the cutting area point list to clipRegionPointsList								
								foreach(NodeItem * node, areas[j]->node_list)
									clipRegionPointsList << QPointF(node->scenePos());
								clipRegionPointsList << QPointF(-1.0, -1.0);
							}
							else if (AreaIntersectArea(mainArea, areas[j])) {  // if main area intersect with clipping area, we use main area to clip the cutting area
								ShapeItem* area_temp = PListToNoAddArea(mainRegionPointsList);
								mainRegionPointsList = ClipAnyRegion(area_temp, areas[j], false);
							}
						}
						mainRegionPointsList << QPointF(-1.0, -1.0);
						mainRegionPointsList.append(clipRegionPointsList);

						ShapeItem* _area = PListToArea(mainRegionPointsList);
						m_shapes << _area; _area->item_select = true;
					}
					DelArea(area); m_shapes.removeOne(area);
					isDeleted = true;
				}
			}
		}
	}
	return isDeleted;
}

void EditableGraphicsScene::InOut(float rate)
{
	if (this->views().length() > 0) {
		QGraphicsView* view = this->views().value(0);
		view->scale(rate, rate);
	}
}

QPointF EditableGraphicsScene::CorrectPointInSceneRect(QPointF point)
{
	QRectF scene_rect = sceneRect();
	double max_x = scene_rect.x() + scene_rect.width();
	double max_y = scene_rect.y() + scene_rect.height();

	double x = 0, y = 0;
	if (point.x() < scene_rect.x())
		x = scene_rect.x();
	else if (point.x() > max_x)
		x = max_x;
	else
		x = point.x();

	if (point.y() < scene_rect.y())
		y = scene_rect.y();
	else if (point.y() > max_y)
		y = max_y;
	else
		y = point.y();

	return QPointF(x, y);
}
/* 应该是将区域的几何信息用一个多边形来表征, 如果有需要裁剪的部分该怎么办呢？ */
QPolygonF EditableGraphicsScene::GetAreaToPolygon(ShapeItem* area)
{
	if (m_shapes.isEmpty()) return QPolygonF();  // 如果区域是空，那么就返回空的多边形

	QPointF point_ref = area->pos();
	QPolygonF poly = area->polygon();
	int count = poly.count();
	if (count <= 2) return QPolygonF(); // 如果区域没有闭合空间，那么就返回空的多边形

	//如果有(-1,-1)怎么办
	QPolygonF poly_merge;
	for (int i = 0; i < count; i++) {
		float x = poly.value(i).x() + point_ref.x();
		float y = poly.value(i).y() + point_ref.y();
		poly_merge << QPointF(x, y);
	}
	//此处有问题，闭合多边形
	if (!poly_merge.isClosed())
		poly_merge << point_ref;
	return poly_merge;
}

QPointF EditableGraphicsScene::GetStartPoint()
{
	if (m_seeds.length() <= 0) return QPointF(-1, -1);

	SeedItem* start_point = m_seeds.last();
	QPointF point = start_point->pos();
	return point;
}

void EditableGraphicsScene::DrawEllipse(QPointF pt1, QPointF pt2, QPointF pt3)
{
	EQUATIONPARA para1, para2;
	GetVertDeuceLine(pt1, pt2, para1);
	GetVertDeuceLine(pt2, pt3, para2);
	QPointF centerPt(0.0, 0.0);
	GetIntersectionPoint(para1, para2, centerPt);

	double dis = GetDistance(pt1.x(), pt1.y(),
		centerPt.x(), centerPt.y());
	dis = dis < 0.5 ? 0.5 : dis;

	ShapeItem* _area = m_shapes.last();
	DelArea(_area);
	m_shapes.removeOne(_area);

	ShapeItem* area_circle = new ShapeItem();
	addItem(area_circle);

	QPointF pos_refer(0.0, 0.0);
	QPointF point_state(0.0, 0.0);
	QPolygonF poly = area_circle->polygon();

	m_shapes << area_circle;
	area_circle->SetNodesVisible(true);

	/* Clip the Polygon by "Reentrant Polygon Clipping" algorithm */
	qreal radius = dis;
	int edges = dis / 6 < 500 ? dis / 6 : 500;
	if (edges < 20) edges = 20;
	qreal deltaAlpha = 2 * PI / edges;
	qreal cosDeltaAlpha = qCos(deltaAlpha);
	qreal sinDletaAlpha = qSin(deltaAlpha);

	QPointF pointNow(0, 0);
	QPointF pointLast(cosDeltaAlpha, sinDletaAlpha);
	QPointF pointLastLast(1.0, 0.0);

	QPointF nodeFirst(1.0, 0.0);
	QPointF nodeSecond(cosDeltaAlpha, sinDletaAlpha);

	QPolygonF oldPoly, newPoly;
	QRectF rect(sceneRect());
	oldPoly.clear(); newPoly.clear();

	oldPoly << radius * nodeFirst + centerPt
		<< radius * nodeSecond + centerPt;

	for (int i = 2; i < edges; i++) {
		pointNow = 2 * cosDeltaAlpha * pointLast - pointLastLast;
		QPointF node(pointNow * radius + centerPt);
		oldPoly << node;
		pointLastLast = pointLast;
		pointLast = pointNow;
	}
	ReentrantPolygonClipping(oldPoly, rect, newPoly);

	edges = newPoly.size();
	for (int i = 0; i < edges; i++) {
		NodeItem* node = new NodeItem(area_circle);
		node->setPos(newPoly.value(i));
		addItem(node);
		area_circle->node_list << node;
		pos_refer = area_circle->node_list.value(0)->scenePos();
		poly << node->scenePos() - pos_refer;
		area_circle->setPos(pos_refer);
		area_circle->setPolygon(poly);
	}
}
/* "Reentrant Polygon Clipping" algorithm. "oldPoly" is the original polygon, the rectangle "rect" clip the polygon into a "newPoly" */
void EditableGraphicsScene::ReentrantPolygonClipping(QPolygonF& oldPoly,
	QRectF rect, QPolygonF& newPoly)
{
	qreal XL = rect.x(), YT = rect.y();
	qreal XR = XL + rect.width(), YB = YT + rect.height();

	int edges = oldPoly.size();

	qreal x, x1, x2, y, y1, y2;
	QPolygonF polyLeft;

	/* cut the polygon by the left boundary */
	for (int i = 0; i < edges; i++) {
		QPointF S(oldPoly.value(i%edges));
		QPointF P(oldPoly.value((i + 1) % edges));

		x1 = S.x(); y1 = S.y();
		x2 = P.x(); y2 = P.y();

		if (S.x() >= XL && P.x() >= XL) { /* Both S and P are visible */
			QPointF node(P);
			polyLeft << node;
		}
		else if (S.x() >= XL && P.x() < XL) {/* S is visible and P is not */
			x = XL;
			y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
			QPointF node(x, y);
			polyLeft << node;
		}
		else if (S.x() < XL && P.x() > XL) { /* P is visible and S is not */
			x = XL;
			y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
			QPointF first(x, y), second(P);
			polyLeft << first << second;
		}
	}

	edges = polyLeft.size();

	QPolygonF polyBottom;

	/* cut the polygon by the bottom boundary */
	for (int i = 0; i < edges; i++) {
		QPointF S(polyLeft.value(i%edges));
		QPointF P(polyLeft.value((i + 1) % edges));

		x1 = S.x(); y1 = S.y();
		x2 = P.x(); y2 = P.y();

		if (S.y() <= YB && P.y() <= YB) {/* Both S and P are visible */
			QPointF node(P);
			polyBottom << node;
		}
		else if (S.y() <= YB && P.y() > YB) {/* S is visible and P is not */
			y = YB;
			x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
			QPointF node(x, y);
			polyBottom << node;
		}
		else if (S.y() > YB && P.y() <= YB) {/* P is visible and S is not */
			y = YB;
			x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
			QPointF first(x, y);
			QPointF second(P);
			polyBottom << first << second;
		}
	}
	edges = polyBottom.size();

	QPolygonF polyRight;
	/* cut the polygon by the Right boundary */
	for (int i = 0; i < edges; i++) {

		QPointF S(polyBottom.value(i%edges));
		QPointF P(polyBottom.value((i + 1) % edges));

		x1 = S.x(); y1 = S.y();
		x2 = P.x(); y2 = P.y();

		if (S.x() <= XR && P.x() <= XR) {
			/* Both S and P are visible */
			QPointF node(P);
			polyRight << node;
		}
		else if (S.x() <= XR && P.x() > XR) {
			/* S is visible and P is not */
			x = XR;
			y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
			QPointF node(x, y);
			polyRight << node;
		}
		else if (S.x() > XR && P.x() <= XR) {
			/* P is visible and S is not */
			x = XR;
			y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
			QPointF first(x, y);
			QPointF second(P);
			polyRight << first << second;
		}
	}

	edges = polyRight.size();

	/* cut the polygon by the Top boundary */
	for (int i = 0; i < edges; i++) {

		QPointF S(polyRight.value(i%edges));
		QPointF P(polyRight.value((i + 1) % edges));

		x1 = S.x(); y1 = S.y();
		x2 = P.x(); y2 = P.y();

		if (S.y() >= YT && P.y() >= YT) {
			/* Both S and P are visible */
			QPointF node(P);
			newPoly << node;
		}
		else if (S.y() >= YT && P.y() < YT) {
			/* S is visible and P is not */
			y = YT;
			x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
			QPointF node(x, y);
			newPoly << node;
		}
		else if (S.y() < YT && P.y() >= YT) {
			/* P is visible and S is not */
			y = YT;
			x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
			QPointF first(x, y);
			QPointF second(P);
			newPoly << first << second;
		}
	}
}

void EditableGraphicsScene::DrawEmptyEllipse(QPointF pt1, QPointF pt2, QPointF pt3)
{
	EQUATIONPARA para1, para2;
	GetVertDeuceLine(pt1, pt2, para1);
	GetVertDeuceLine(pt2, pt3, para2);
	QPointF centerPt(0.0, 0.0);
	GetIntersectionPoint(para1, para2, centerPt);

	double dis = GetDistance(pt1.x(), pt1.y(),
		centerPt.x(), centerPt.y());

	removeItem(m_shapes.last());
	m_shapes.removeLast();
	ShapeItem* area_circle = new ShapeItem();
	area_circle->transparency_count = 0;
	addItem(area_circle);

	QPointF pos_refer(0.0, 0.0), point_state(0.0, 0.0);
	QPolygonF poly = area_circle->polygon();
	m_shapes << area_circle;

	int edges = dis / 6;
	edges = edges < 20 ? 20 : edges;

	for (double i = 0.0; i < 2 * PI; i = i + 2 * PI / edges)
	{
		point_state.setX(centerPt.x() + dis * qCos(i));
		point_state.setY(centerPt.y() + dis * qSin(i));

		QPointF add_point_state = CorrectPointInSceneRect(point_state);
		int count = area_circle->node_list.length();
		if (count == 0) {
			NodeItem* node = new NodeItem(area_circle);
			node->setPos(add_point_state);
			area_circle->node_list << node;
			pos_refer = area_circle->node_list.value(0)->scenePos();
			poly << node->scenePos() - pos_refer;
			area_circle->setPos(pos_refer);
			area_circle->setPolygon(poly);
		}
		else {
			double x1 = area_circle->node_list.last()->scenePos().x();
			double y1 = area_circle->node_list.last()->scenePos().y();
			double x2 = add_point_state.x();
			double y2 = add_point_state.y();

			if (!((x1 == x2) || (y1 == y2))) {
				NodeItem* node = new NodeItem(area_circle);
				node->setPos(add_point_state);
				area_circle->node_list << node;
				pos_refer = area_circle->node_list.value(0)->scenePos();
				poly << node->scenePos() - pos_refer;
				area_circle->setPos(pos_refer);
				area_circle->setPolygon(poly);
			}
		}
	}
}

void EditableGraphicsScene::GetVertDeuceLine(QPointF pt1, QPointF pt2, EQUATIONPARA &para)
{
	QPoint   cPt;
	cPt.setX((pt1.x() + pt2.x()) / 2);
	cPt.setY((pt1.y() + pt2.y()) / 2);
	if (pt1.x() == pt2.x())//垂线 
	{
		para.a = 0;
		para.b = 1;
		para.c = -cPt.y();
		return;
	}
	if (pt1.y() == pt2.y())
	{
		para.b = 0;
		para.a = 1;
		para.c = -cPt.x();
		return;
	}
	para.b = 1;
	if ((pt1.y() - pt2.y()) != 0)
		para.a = ((double)(pt1.x() - pt2.x())) / (pt1.y() - pt2.y());

	para.c = -(para.a*cPt.x() + cPt.y());
}

bool EditableGraphicsScene::GetIntersectionPoint(EQUATIONPARA para1,
	EQUATIONPARA para2, QPointF &center_point)
{
	if (para1.a == para2.a   &&   para1.b == para2.b)
		return   false;
	if (para1.b == 0 && para2.a == 0)
	{
		center_point.setX(-para1.c);
		center_point.setY(-para2.c);
	}
	else if (para1.a == 0 && para2.b == 0)
	{
		center_point.setX(-para2.c);
		center_point.setY(-para1.c);
	}
	else
	{
		center_point.setX((para2.b*para1.c - para1.b*para2.c) / (para2.a*para1.b - para1.a*para2.b));
		center_point.setY((para1.a*para2.c - para2.a*para1.c) / (para2.a*para1.b - para1.a*para2.b));
	}
	return  true;
}

double EditableGraphicsScene::GetDistance(long x1, long y1, long x2, long y2)
{
	return qSqrt((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1));
}

bool EditableGraphicsScene::IsIntersect(QPointF P11, QPointF P12, QPointF P21, QPointF P22, QPointF P)
{

	double x11 = P11.x();
	double y11 = P11.y();
	double x12 = P12.x();
	double y12 = P12.y();
	double x21 = P21.x();
	double y21 = P21.y();
	double x22 = P22.x();
	double y22 = P22.y();
	double k1 = 1.0*(y12 - y11) / (x12 - x11);
	double k2 = 1.0*(y22 - y21) / (x22 - x21);
	double x, y;
	if (k1 == k2 || (x11 == x12 && x21 == x22))//如果斜率相等或同为正无穷
		return false;

	if (k1 == 0 && x21 != x22)//线1水平，线2倾斜
	{
		y = y11;
		x = (y11 - y21) / k2 + x21;
	}
	else if (k2 == 0 && x11 != x12)//线2水平，线1倾斜
	{
		y = y21;
		x = (y21 - y11) / k1 + x11;
	}
	else if (x11 == x12 && x21 != x22)//线1垂直，线2倾斜
	{
		x = x11;
		y = k2 * (x11 - x21) + y21;
	}
	else if (x21 == x22 && x11 != x12)//线2垂直，线1倾斜
	{
		x = x21;
		y = k1 * (x21 - x11) + y11;
	}
	else if (k1 == 0 && x21 == x22)//线1平行，线2垂直
	{
		x = x11;
		y = y21;
	}
	else if (k2 == 0 && x11 == x12)//线2平行，线1垂直
	{
		x = x21;
		y = y11;
	}
	else
	{
		x = (y21 - y11 + k1 * x11 - k2 * x21) / (k1 - k2);
		y = k1 * (x - x11) + y11;
	}
	if (((x >= x11 && x <= x12) || (x <= x11 && x >= x12)) &&
		((x >= x21 && x <= x22) || (x <= x21 && x >= x22)) &&
		((y >= y11 && y <= y12) || (y <= y11 && y >= y12)) &&
		((y >= y21 && y <= y22) || (y <= y21 && y >= y22)))
	{
		P = QPointF(x, y);
		return true;
	}
	return false;
}

QList<QPointF> EditableGraphicsScene::ClipAnyRegion(ShapeItem* areaMain,
	ShapeItem* areaClip, bool clipType)//返回裁减结果区域的顶点序列,多个区域之间以两个QPointF(0,0)隔开
/* I think this is Weiler-Atherton algorithm */
{
	int i, j, k, clipbz1, clipbz2, clipResuAreaNum = 0;
	ClipRegion Reg, Reg1, Reg2, *p, *q, *Reg1First, *Reg2First, *r1, *r2, *s2;
	QList<QPointF> pointResultList;//记录剪切结果区域的节点位置
	double x1, y1, x2, y2, x3, y3, x4, y4, xx[2], yy[2];
	clipbz1 = 1;   // this may be the flag of entering point
	clipbz2 = -1;  // this may be the flag of leaving point
	if (CalArea(areaMain) > 0.0)//将被裁剪多边形按顺时针方向排列,若>0.0为逆时针方向,则须转换方向
		ConvertAreaNodeSort(areaMain);
	if (CalArea(areaClip) > 0.0)//将裁剪多边形按顺时针方向排列,若>0.0为逆时针方向,则须转换方向
		ConvertAreaNodeSort(areaClip);

	p = &Reg1; //M0->M1->M2->M0
	for (i = 0; i < areaMain->node_list.length(); i++)//建立主（被）裁剪区域链表
	{
		if (i == 0) {
			p->marker = "M" + QString::number(i, 10);
			p->x = areaMain->node_list.value(i)->scenePos().x();
			p->y = areaMain->node_list.value(i)->scenePos().y();
			p->flag = 0;
			Reg1First = new ClipRegion;   // Reg1First maybe the first point of the polygon
			Reg1First->marker = "M" + QString::number(i, 10);
			Reg1First->x = areaMain->node_list.value(i)->scenePos().x();
			Reg1First->y = areaMain->node_list.value(i)->scenePos().y();
			Reg1First->flag = 0;
			Reg1First->next = nullptr;
		}
		else {
			q = new ClipRegion;
			q->marker = "M" + QString::number(i % (areaMain->node_list.length()), 10);
			q->x = areaMain->node_list.value(i)->scenePos().x();
			q->y = areaMain->node_list.value(i)->scenePos().y();
			q->flag = 0;
			q->next = nullptr;
			p->next = q;
			p = p->next;
		}
	}
	p->next = Reg1First;  // the main list is a circular linked list

	p = &Reg2; //C0->C1->C2->C0
	for (i = 0; i < areaClip->node_list.length(); i++) //建立裁剪区域链表
	{
		if (i == 0) {
			p->marker = "C" + QString::number(i, 10);
			p->x = areaClip->node_list.value(i)->scenePos().x();
			p->y = areaClip->node_list.value(i)->scenePos().y();
			if (IsInRegion(p->x, p->y, areaMain)) //若点在被裁剪区域内，则为“出”点
				p->flag = clipbz2;  // 
			else
				p->flag = 0;
			Reg2First = new ClipRegion;
			Reg2First->marker = "C" + QString::number(i, 10);
			Reg2First->x = areaClip->node_list.value(i)->scenePos().x();
			Reg2First->y = areaClip->node_list.value(i)->scenePos().y();
			if (IsInRegion(Reg2First->x, Reg2First->y, areaMain))
				Reg2First->flag = clipbz2;
			else
				Reg2First->flag = 0;
			Reg2First->next = nullptr;
		}
		else {
			q = new ClipRegion;
			q->marker = "C" + QString::number(i % (areaClip->node_list.length()), 10);
			q->x = areaClip->node_list.value(i)->scenePos().x();
			q->y = areaClip->node_list.value(i)->scenePos().y();
			if (this->IsInRegion(q->x, q->y, areaMain))//若点在被裁剪区域内，则为“出”点
				q->flag = clipbz2;
			else q->flag = 0;
			q->next = nullptr;
			p->next = q;
			p = p->next;
		}
	}
	p->next = Reg2First;

	//求取主裁剪多边形区域与裁剪多边形区域的交点，并将交点按位置顺序加入链表
	p = &Reg1;
	int dds = 0, bz, ptn = 0;
	double xa, ya, lamda;
	double xls[50], yls[50], djl[50]; //记录被裁剪多边形任一边与裁剪多边形的交点
	QString dhz[50], markerList[500];
	int jdwz[500];  //  the number of the clip polygon
	double xxls[500], yyls[500];//记录裁剪多边形与主裁剪多边形的总交点
	bool isCross = false;

	for (i = 0; i < areaMain->node_list.length(); i++) {
		x1 = p->x, y1 = p->y;
		x2 = p->next->x, y2 = p->next->y;
		ptn = 0;  // ptn is the number of intersections of main region edge (x1,y1),(x2,y2) with clip region edges
		q = &Reg2;
		for (j = 0; j < areaClip->node_list.length(); j++)
		{
			x3 = q->x;               // (x1,y1) and (x2,y2) is an edge of the main region
			y3 = q->y;               // (x3,y3) and (x4,y4) is an edge of the clip region
			x4 = q->next->x;
			y4 = q->next->y;
			bool bF1, bF2, bF3, bF4;
			bool bFu1, bFu2, bFu3, bFu4;

			bF1 = fabs(x3 - x1) < 0.0000001 && fabs(y3 - y1) < 0.000001;         // if (x1,y1) and (x3,y3) are same, bF1 is true.
			bF2 = fabs(x4 - x1) < 0.0000001 && fabs(y4 - y1) < 0.000001;         // if (x1,y1) and (x4,y4) are same, bF2 is true.
			bF3 = fabs(x3 - x2) < 0.0000001 && fabs(y3 - y2) < 0.000001;         // if (x2,y2) and (x3,y3) are same, bF3 is true.
			bF4 = fabs(x4 - x2) < 0.0000001 && fabs(y4 - y2) < 0.000001;         // if (x2,y2) and (x4,y4) are same, bF4 is true.

			double dite, funcf1, funcf2, funcf3, funcf4;
			dite = x2 - x1;
			if (fabs(dite) < 0.00001)
			{ /* if the line between (x1,y1) and (x2,y2) is vertical */
				funcf1 = x3 - x1;
				funcf2 = x4 - x1;
			}
			else
			{
				funcf1 = (y3 - y1) - (y2 - y1) * (x3 - x1) / dite;             //  funcf/(x-x1) = (y-y1)/(x-x1) - (y2-y1)/(x2-x1)
				funcf2 = (y4 - y1) - (y2 - y1) * (x4 - x1) / dite;             //  if "funcf" is small, we can regard (x1,y1), (x2,y2) and (x,y) in the same line.
			}
			dite = x4 - x3;
			if (fabs(dite) < 0.00001)
			{ /* if the line between (x3,y3) and (x4,y4) is vertical */
				funcf3 = x1 - x3;
				funcf4 = x2 - x3;
			}
			else
			{
				funcf3 = (y1 - y3) - (y4 - y3) * (x1 - x3) / dite;
				funcf4 = (y2 - y3) - (y4 - y3) * (x2 - x3) / dite;
			}

			/* if (x3,y3)  in the segment (x1,y1) and (x2,y2), bFu1 is true */
			bFu1 = fabs(funcf1) < 0.0001 &&
				x3 >= std::min(x1, x2) - 0.000001 &&
				x3 <= std::max(x1, x2) + 0.0000001 &&
				y3 >= std::min(y1, y2) - 0.000001 &&
				y3 <= std::max(y1, y2) + 0.0000001;
			/* if (x4,y4)  in the segment (x1,y1) and (x2,y2), bFu2 is true */
			bFu2 = fabs(funcf2) < 0.0001 &&
				x4 >= std::min(x1, x2) - 0.000001 &&
				x4 <= std::max(x1, x2) + 0.0000001 &&
				y4 >= std::min(y1, y2) - 0.000001 &&
				y4 <= std::max(y1, y2) + 0.0000001;
			/* if (x1,y1)  in the segment (x3,y3) and (x4,y4), bFu3 is true */
			bFu3 = fabs(funcf3) < 0.0001 &&
				x1 >= std::min(x3, x4) - 0.000001 &&
				x1 <= std::max(x3, x4) + 0.0000001&&
				y1 >= std::min(y3, y4) - 0.000001&&
				y1 <= std::max(y3, y4) + 0.0000001;
			/* if (x2,y2)  in the segment (x3,y3) and (x4,y4), bFu4 is true */
			bFu4 = fabs(funcf4) < 0.0001 &&
				x2 >= std::min(x3, x4) - 0.000001 &&
				x2 <= std::max(x3, x4) + 0.0000001 &&
				y2 >= std::min(y3, y4) - 0.000001 &&
				y2 <= std::max(y3, y4) + 0.0000001;

			if (bF1 || bF2 || bF3 || bF4 || bFu1 || bFu2 || bFu3 || bFu4)         // special cases
			{
				if (bF1)//(x1,y1)与(x3,y3)为同一点
				{
					p->marker = q->marker;
					xa = (x1 + 0.002 * x2) / (1.002);             // it seems that (xa,ya) is a very close point to (x1,y1)
					ya = (y1 + 0.002 * y2) / (1.002);
					bz = IsInRegion(xa, ya, areaClip);
					if (bz)
						q->flag = clipbz1;//入点
					else
						q->flag = clipbz2;//出点
					p->flag = q->flag;
				}
				else if (bFu1 && (!bF2) && (!bF3)) //(x3,y3)点在(x1,y1)--(x2,y2)边上
				{
					xls[ptn] = x3;
					yls[ptn] = y3;
					dhz[ptn] = q->marker;
					ptn++;
				}
				else if (bFu3 && (!bF2) && (!bF3) && j != areaClip->node_list.length() - 1)//(x1,y1)点在(x3,y3)--(x4,y4)边上//length()-2可能有问题
				{
					xxls[dds] = x1;
					yyls[dds] = y1;
					jdwz[dds] = j;
					markerList[dds] = p->marker;
					xa = (x1 + 0.002 * x2) / (1.002);
					ya = (y1 + 0.002 * y2) / (1.002);
					bz = IsInRegion(xa, ya, areaClip);
					if (bz)
						q->flag = clipbz1;//入点
					else
						q->flag = clipbz2;//出点
					p->flag = q->flag;
					dds++;
				}
			}
			else if (CalLineJoint(x1, y1, x2, y2, x3, y3, x4, y4, xx, yy))  // (xx,yy) is the intersection of main region edge (x1,y1),(x2,y2) and clip region edge (x3,y3),(x4,y4)
			{
				xls[ptn] = xx[0];
				yls[ptn] = yy[0];
				xxls[dds] = xx[0];
				yyls[dds] = yy[0];
				jdwz[dds] = j;
				markerList[dds] = "J" + QString::number(dds + 1, 10);  // why plus 1 ?
				dhz[ptn] = "J" + QString::number(dds + 1, 10);
				ptn++;
				dds++;
			}
			q = q->next;
		}

		if (ptn > 0)//将交点插入被裁剪多边形链表中
		{
			isCross = true;
			if (ptn == 1)//交点只有一个，则直接插入链表
			{
				r1 = new ClipRegion;
				r1->x = xls[0];
				r1->y = yls[0];
				r1->marker = dhz[0];
				lamda = 0.002;
				xa = (xls[0] + lamda * x2) / (1.0 + lamda);
				ya = (yls[0] + lamda * y2) / (1.0 + lamda);
				bz = IsInRegion(xa, ya, areaClip);
				xa = (xls[0] + lamda * x1) / (1.0 + lamda);
				ya = (yls[0] + lamda * y1) / (1.0 + lamda);
				int bbbz = IsInRegion(xa, ya, areaClip);
				if (bbbz != bz)
				{
					if (bz)
						r1->flag = clipbz1;//入点
					else
						r1->flag = clipbz2;//出点
				}
				else
					r1->flag = 5;
				r1->next = p->next;
				p->next = r1;
				p = r1;
			}
			else  //若交点大于一个，则对交点根据其与(x1,y1)点的距离，确定其顺序
			{
				for (k = 0; k < ptn; k++)//计算交点到(x1,y1)的距离
				{
					djl[k] = qSqrt((xls[k] - x1)*(xls[k] - x1) + (yls[k] - y1)*(yls[k] - y1));
				}
				for (k = 0; k < ptn - 1; k++)//交点按距离(x1,y1)点进行排序
				{ /* this is a bubble sort algorithm */
					int min = k;
					for (int l = k + 1; l < ptn; l++)
					{
						if (djl[l] < djl[min])
							min = l;
					}
					double temp = djl[k];
					djl[k] = djl[min];
					djl[min] = temp;

					temp = xls[k];
					xls[k] = xls[min];
					xls[min] = temp;
					temp = yls[k];
					yls[k] = yls[min];
					yls[min] = temp;

					QString dlss = dhz[k];
					dhz[k] = dhz[min];
					dhz[min] = dlss;
				}
				for (k = 0; k < ptn; k++)//将交点插入链表中
				{
					r1 = new ClipRegion;
					r1->x = xls[k];
					r1->y = yls[k];
					r1->marker = dhz[k];
					lamda = 0.002;
					xa = (xls[k] + lamda * x2) / (1.0 + lamda);                      // (xa,ya) is a point very close to (xls[k],yls[k]) and in the segment (xls[k],yls[k]) and (x2,y2)
					ya = (yls[k] + lamda * y2) / (1.0 + lamda);
					bz = IsInRegion(xa, ya, areaClip);
					xa = (xls[k] + lamda * x1) / (1.0 + lamda);                      // (xa,ya) is a point very close to (xls[k],yls[k]) and in the segment (xls[k],yls[k]) and (x1,y1)
					ya = (yls[k] + lamda * y1) / (1.0 + lamda);
					int bbbz = IsInRegion(xa, ya, areaClip);
					if (bbbz != bz)
					{
						if (bz)
							r1->flag = clipbz1;//入点
						else
							r1->flag = clipbz2;//出点
					}
					else
						r1->flag = 5;
					r1->next = p->next;
					p->next = r1;
					p = r1;
				}
			}
		}

		p = p->next;
	}

	//对裁剪多边形区域点按顺序排列
	q = &Reg2;
	for (k = 0; k < dds; k++) //交点按边的顺序进行排序
	{
		int min = k;
		for (int l = k + 1; l < dds; l++)
		{
			if (jdwz[l] < jdwz[min])
				min = l;
		}
		int temp = jdwz[k];
		jdwz[k] = jdwz[min];
		jdwz[min] = temp;

		QString cs = markerList[k];
		markerList[k] = markerList[min];
		markerList[min] = cs;

		double temp1 = xxls[k];
		xxls[k] = xxls[min];
		xxls[min] = temp1;
		temp1 = yyls[k];
		yyls[k] = yyls[min];
		yyls[min] = temp1;
	}

	int lsjds, jdqd;
	for (i = 0; i < areaClip->node_list.length(); i++) //对裁剪多边形每一边上有无交点进行判断，若有交点则进行插入链表中
	{
		x1 = q->x;
		y1 = q->y;
		lsjds = 0;
		jdqd = 0;
		for (j = 0; j < dds; j++)
		{
			if (jdwz[j] == i)
			{
				if (lsjds == 0)
					jdqd = j;
				lsjds++;
			}
		}
		if (lsjds > 0)//有交点
		{
			if (lsjds == 1)//1个交点直接插入
			{
				r2 = new ClipRegion;
				r2->x = xxls[jdqd];
				r2->y = yyls[jdqd];
				r2->marker = markerList[jdqd];
				r2->flag = -100;
				r2->next = q->next;
				q->next = r2;
				q = r2;
			}
			else//大于一个交点
			{
				for (k = 0; k < lsjds; k++)//计算交点到(x1,y1)的距离
				{
					djl[k] = qSqrt((xxls[jdqd + k] - x1)*(xxls[jdqd + k] - x1) + (yyls[jdqd + k] - y1)*(yyls[jdqd + k] - y1));
				}
				for (k = 0; k < lsjds - 1; k++)//交点按距离(x1,y1)点进行排序
				{
					int min = k;
					for (int l = k + 1; l < lsjds; l++)
					{
						if (djl[l] < djl[min])
							min = l;
					}
					double temp = djl[k];
					djl[k] = djl[min];
					djl[min] = temp;

					temp = xxls[jdqd + k];
					xxls[jdqd + k] = xxls[jdqd + min];
					xxls[jdqd + min] = temp;
					temp = yyls[jdqd + k];
					yyls[jdqd + k] = yyls[jdqd + min];
					yyls[jdqd + min] = temp;
					QString cs = markerList[jdqd + k];
					markerList[jdqd + k] = markerList[jdqd + min];
					markerList[jdqd + min] = cs;
				}
				for (k = 0; k < lsjds; k++)
				{
					r2 = new ClipRegion;
					r2->x = xxls[jdqd + k];
					r2->y = yyls[jdqd + k];
					r2->marker = markerList[jdqd + k];
					r2->flag = -100;
					r2->next = q->next;
					q->next = r2;
					q = r2;
				}
			}
		}
		q = q->next;
	}

	//对交点是“入”、“出”点标记
	q = &Reg2;
	while (q->next != 0)
	{
		if (q->flag == -100)  // this is an intersection of the clip region
		{
			p = &Reg1;
			while (QString::compare(q->marker, p->marker) != 0 && p->next != 0)//与被裁剪多边形交点为同一点，则标记为同一标志值
				p = p->next;
			q->flag = p->flag;
		}
		q = q->next;
	}
	q = &Reg2;
	while (q->next != 0) //与被裁剪多边形同一点名者，标记为同一“入”、“出”点标志
	{
		p = &Reg1;
		while (QString::compare(q->marker, p->marker) != 0 && p->next != 0)
			p = p->next;
		q->flag = p->flag;
		q = q->next;
	}

	p = &Reg1;
	QString staDh = p->marker;
	int flbz = p->flag;
	while (p != nullptr)//将被裁剪多边形的首末点标记为同一点名、同一“入”、“出”点标记//nullptr
	{
		p = p->next;
		if (p != nullptr && p->next == nullptr)
		{
			p->marker = staDh;
			p->flag = flbz;
		}
	}
	p = &Reg2;
	staDh = p->marker;
	flbz = p->flag;
	while (p != nullptr)//将裁剪多边形的首末点标记为同一点名、同一“入”、“出”点标记//nullptr
	{
		p = p->next;
		if (p != nullptr && p->next == nullptr)
		{
			p->marker = staDh;
			p->flag = flbz;
		}
	}

	//形成裁剪多边形区域
	if (!clipType)//外裁剪,将裁剪区域按逆时针方向排列
	{
		p = &Reg2;
		ptn = 0;
		while (p != 0)
		{
			markerList[ptn] = p->marker;
			jdwz[ptn] = p->flag;
			xxls[ptn] = p->x;
			yyls[ptn] = p->y;
			ptn++;
			p = p->next;
		}
		p = &Reg2;
		int lsdty = 0;
		while (p != 0)
		{
			p->marker = markerList[ptn - 1 - lsdty];
			p->flag = jdwz[ptn - 1 - lsdty];
			p->x = xxls[ptn - 1 - lsdty];
			p->y = yyls[ptn - 1 - lsdty];
			lsdty++;
			p = p->next;
		}
		//*/
		clipbz1 = -1;
		clipbz2 = 1;
	}

	//将链表首尾相连
	int pt1, pt2, runbz = 1; //pt1+1=被裁减多边形上点的个数,包括交点和多边形顶点;pt2+1=裁减多边形上点的个数,包括交点和多边形顶点
	pt1 = 0;
	p = &Reg1;
	while (p->next->next != 0)
	{
		pt1++;
		p = p->next;
	}
	p->next = &Reg1;  // make this main region list a circular list
	pt2 = 0;
	q = &Reg2;
	while (q->next->next != 0)
	{
		pt2++;
		q = q->next;
	}
	q->next = &Reg2;  // make the clip region list a circular list 

	clipResuAreaNum = 0;//区域数
	int ppz = 1, jlqds = 0;
	while (runbz)
	{
		runbz = 0;
		p = &Reg1;
		ptn = 0;
		for (int lsi = 0; lsi <= pt1; lsi++)
		{  // p pointer to the current, q pointer to the last
			q = p;
			if (p->flag == clipbz1) // p is an entering point
			{
				runbz = 1;
				jlqds = lsi;
				lsi = pt1 + 5;  // because lsi = pt1 + 5 > pt1, next time will jump out this loop
			}
			p = p->next;
		}
		if (runbz)
		{
			while (q != 0)
			{
				if (q->flag == clipbz1)//"入"点，沿着被裁剪多边形搜索顶点序列
				{
					ppz = 2;
					xxls[ptn] = q->x;
					yyls[ptn] = q->y;
					markerList[ptn] = q->marker;
					q->flag = 0;
					ptn++;
					s2 = &Reg2;
					for (int lsi = 0; lsi <= pt2; lsi++)
					{
						if (QString::compare(s2->marker, q->marker) == 0)
							s2->flag = 0;
						s2 = s2->next;
					}
					q = q->next;
				}
				else if (q->flag == clipbz2)//"出"点,沿着裁剪多边形搜索顶点序列
				{
					ppz = 1;
					xxls[ptn] = q->x;
					yyls[ptn] = q->y;
					markerList[ptn] = q->marker;
					q->flag = 0;
					ptn++;
					s2 = &Reg2;
					for (int lsi = 0; lsi <= pt2; lsi++)
					{
						if (QString::compare(s2->marker, q->marker) == 0)//标记相同点名为相同的标志
						{
							s2->flag = 0;
							lsi = pt2 + 5;
						}
						s2 = s2->next;
					}
					if (QString::compare(s2->marker, q->marker) == 0)
						s2 = s2->next;
					while (s2 != 0)
					{
						if (s2->flag == clipbz1)
						{
							r1 = &Reg1;
							while (QString::compare(s2->marker, r1->marker) != 0)
								r1 = r1->next;
							ppz = 2;
							q = r1;
							s2 = 0;
						}
						else
						{
							xxls[ptn] = s2->x;
							yyls[ptn] = s2->y;
							markerList[ptn] = s2->marker;
							s2->flag = 0;
							ptn++;

							if (QString::compare(markerList[0], markerList[ptn - 1]) == 0)//首末点点名相同时裁剪出一个区域
							{
								s2 = 0;
							}
							else
							{
								r1 = &Reg1;
								for (int lsi = 0; lsi <= pt1; lsi++)
								{
									if (QString::compare(s2->marker, r1->marker) == 0)//标记相同点名为相同的标志
									{
										r1->flag = 0;
										lsi = pt1 + 5;
									}
									r1 = r1->next;
								}
								s2 = s2->next;
							}
						}
					}
				}
				else
				{
					if (ppz == 2)//前方距离最近的交点为"入"点
					{
						xxls[ptn] = q->x;
						yyls[ptn] = q->y;
						markerList[ptn] = q->marker;
						q->flag = 0;
						ptn++;
						s2 = &Reg2;
						for (int lsi = 0; lsi <= pt2; lsi++)
						{
							if (QString::compare(s2->marker, q->marker) == 0)//标记相同点名为相同的标志
								s2->flag = 0;
							s2 = s2->next;
						}
						q = q->next;
					}
					else//前一个点不是"入"点
						q = q->next;
				}
				if (ptn > 1)
				{
					if (QString::compare(markerList[0], markerList[ptn - 1]) == 0)//首末点点名相同时裁剪出一个区域
					{
						for (i = 0; i < ptn - 1; i++)
						{
							QPointF point = QPointF(xxls[i], yyls[i]);
							pointResultList << point;
						}
						pointResultList << QPointF(-1.0, -1.0); //<<QPointF(0,0);
						clipResuAreaNum++;
						q = 0;
					}
				}

			}
		}
	}

	if (pointResultList.empty() && isCross == false)
	{
		bool isInplygon = false;
		for (i = 0; i < areaClip->node_list.length(); i++)
		{
			double x = areaClip->node_list.value(i)->scenePos().x();
			double y = areaClip->node_list.value(i)->scenePos().y();
			if (!IsInRegion(x, y, areaMain))
				isInplygon = false;
			else
				isInplygon = true;
		}
		if (isInplygon)
		{
			for (i = 0; i < areaMain->node_list.length(); i++)
			{
				QPointF point = QPointF(areaMain->node_list.value(i)->scenePos().x(), areaMain->node_list.value(i)->scenePos().y());
				pointResultList << point;
			}
			pointResultList << QPointF(-1.0, -1.0); //<<QPointF(0,0);
			for (i = areaClip->node_list.length() - 1; i >= 0; i--)
			{
				QPointF point = QPointF(areaClip->node_list.value(i)->scenePos().x(), areaClip->node_list.value(i)->scenePos().y());
				pointResultList << point;
			}

		}

	}

	PListToNormalize(pointResultList);

	return pointResultList;
}

QList<QPointF> EditableGraphicsScene::AddAnyRegion(ShapeItem* areaMain,
	ShapeItem* areaClip, bool clipType)//返回合并结果区域的顶点序列,多个区域之间以两个QPointF(0,0)隔开
{
	int clipResuAreaNum = 0;
	ClipRegion Reg, Reg1, Reg2, *p, *q, *Reg1First, *Reg2First, *r1, *r2, *s2;
	QList<QPointF > pointResultList; //记录合并结果区域的节点位置
	int i, j, k, clipbz1, clipbz2;
	double x1, y1, x2, y2, x3, y3, x4, y4, xx[2], yy[2];
	clipbz1 = 1;
	clipbz2 = -1;
	if (CalArea(areaMain) > 0.0) //将被合并多边形按顺时针方向排列,若>0.0为顺时针方向,则须转换方向
		ConvertAreaNodeSort(areaMain);
	if (CalArea(areaClip) > 0.0) //将合并多边形按顺时针方向排列,若>0.0为顺时针方向,则须转换方向
		ConvertAreaNodeSort(areaClip);

	p = &Reg1; //M0->M1->M2->M0
	for (i = 0; i < areaMain->node_list.length(); i++)//建立主（被）合并区域链表
	{
		if (i == 0)
		{
			p->marker = "M" + QString::number(i, 10);
			p->x = areaMain->node_list.value(i)->scenePos().x();
			p->y = areaMain->node_list.value(i)->scenePos().y();
			p->flag = 0;
			Reg1First = new ClipRegion;
			Reg1First->marker = "M" + QString::number(i, 10);
			Reg1First->x = areaMain->node_list.value(i)->scenePos().x();
			Reg1First->y = areaMain->node_list.value(i)->scenePos().y();
			Reg1First->flag = 0;
			Reg1First->next = nullptr;
		}
		else {
			q = new ClipRegion;
			q->marker = "M" + QString::
				number(i % (areaMain->node_list.length()), 10);
			q->x = areaMain->node_list.value(i)->scenePos().x();
			q->y = areaMain->node_list.value(i)->scenePos().y();
			q->flag = 0;
			q->next = nullptr;
			p->next = q;
			p = p->next;
		}
	}
	p->next = Reg1First;

	p = &Reg2; //C0->C1->C2->C0
	for (i = 0; i < areaClip->node_list.length(); i++)//建立合并区域链表
	{
		if (i == 0) {
			p->marker = "C" + QString::number(i, 10);
			p->x = areaClip->node_list.value(i)->scenePos().x();
			p->y = areaClip->node_list.value(i)->scenePos().y();
			if (IsInRegion(p->x, p->y, areaMain)) //若点在被合并区域内，则为“出”点
				p->flag = clipbz2;
			else
				p->flag = 0;
			Reg2First = new ClipRegion;
			Reg2First->marker = "C" + QString::number(i, 10);
			Reg2First->x = areaClip->node_list.value(i)->scenePos().x();
			Reg2First->y = areaClip->node_list.value(i)->scenePos().y();
			if (IsInRegion(Reg2First->x, Reg2First->y, areaMain))
				Reg2First->flag = clipbz2;
			else
				Reg2First->flag = 0;
			Reg2First->next = nullptr;
		}
		else {
			q = new ClipRegion;
			q->marker = "C" + QString::number(i % (areaClip->node_list.length()), 10);
			q->x = areaClip->node_list.value(i)->scenePos().x();
			q->y = areaClip->node_list.value(i)->scenePos().y();
			if (this->IsInRegion(q->x, q->y, areaMain))//若点在被合并区域内，则为“出”点
				q->flag = clipbz2;
			else
				q->flag = 0;
			q->next = nullptr;
			p->next = q;
			p = p->next;
		}
	}
	p->next = Reg2First;

	//求取主合并多边形区域与合并多边形区域的交点，并将交点按位置顺序加入链表
	p = &Reg1;
	int dds = 0, bz, ptn = 0;
	double xa, ya, lamda;
	double xls[50], yls[50], djl[50];//记录被合并多边形任一边与合并多边形的交点
	QString dhz[50], markerList[500];
	int jdwz[500];
	double xxls[500], yyls[500];//记录合并多边形与主合并多边形的总交点

	for (i = 0; i < areaMain->node_list.length(); i++) {
		x1 = p->x;
		y1 = p->y;
		x2 = p->next->x;
		y2 = p->next->y;
		ptn = 0;
		q = &Reg2;
		for (j = 0; j < areaClip->node_list.length(); j++) {
			x3 = q->x;
			y3 = q->y;
			x4 = q->next->x;
			y4 = q->next->y;
			bool bF1, bF2, bF3, bF4;
			bool bFu1, bFu2, bFu3, bFu4;

			bF1 = fabs(x3 - x1) < 0.0000001 && fabs(y3 - y1) < 0.000001;
			bF2 = fabs(x4 - x1) < 0.0000001 && fabs(y4 - y1) < 0.000001;
			bF3 = fabs(x3 - x2) < 0.0000001 && fabs(y3 - y2) < 0.000001;
			bF4 = fabs(x4 - x2) < 0.0000001 && fabs(y4 - y2) < 0.000001;

			double dite, funcf1, funcf2, funcf3, funcf4;
			dite = x2 - x1;
			if (fabs(dite) < 0.00001) {
				funcf1 = x3 - x1;
				funcf2 = x4 - x1;
			}
			else {
				funcf1 = (y3 - y1) - (y2 - y1) * (x3 - x1) / dite;
				funcf2 = (y4 - y1) - (y2 - y1) * (x4 - x1) / dite;
			}
			dite = x4 - x3;
			if (fabs(dite) < 0.00001) {
				funcf3 = x1 - x3;
				funcf4 = x2 - x3;
			}
			else {
				funcf3 = (y1 - y3) - (y4 - y3) * (x1 - x3) / dite;
				funcf4 = (y2 - y3) - (y4 - y3) * (x2 - x3) / dite;
			}

			bFu1 = fabs(funcf1) < 0.0001 &&
				x3 >= std::min(x1, x2) - 0.000001 &&
				x3 <= std::max(x1, x2) + 0.0000001 &&
				y3 >= std::min(y1, y2) - 0.000001 &&
				y3 <= std::max(y1, y2) + 0.0000001;
			bFu2 = fabs(funcf2) < 0.0001 &&
				x4 >= std::min(x1, x2) - 0.000001 &&
				x4 <= std::max(x1, x2) + 0.0000001 &&
				y4 >= std::min(y1, y2) - 0.000001 &&
				y4 <= std::max(y1, y2) + 0.0000001;
			bFu3 = fabs(funcf3) < 0.0001 &&
				x1 >= std::min(x3, x4) - 0.000001 &&
				x1 <= std::max(x3, x4) + 0.0000001 &&
				y1 >= std::min(y3, y4) - 0.000001 &&
				y1 <= std::max(y3, y4) + 0.0000001;
			bFu4 = fabs(funcf4) < 0.0001 &&
				x2 >= std::min(x3, x4) - 0.000001 &&
				x2 <= std::max(x3, x4) + 0.0000001 &&
				y2 >= std::min(y3, y4) - 0.000001 &&
				y2 <= std::max(y3, y4) + 0.0000001;

			if (bF1 || bF2 || bF3 || bF4 || bFu1 || bFu2 || bFu3 || bFu4) {
				if (bF1)//(x1,y1)与(x3,y3)为同一点
				{
					p->marker = q->marker;
					xa = (x1 + 0.002 * x2) / (1.002);
					ya = (y1 + 0.002 * y2) / (1.002);
					bz = IsInRegion(xa, ya, areaClip);
					if (bz)
						q->flag = clipbz1;//入点
					else
						q->flag = clipbz2;//出点
					p->flag = q->flag;
				}
				else if (bFu1 && (!bF2) && (!bF3))//(x3,y3)点在(x1,y1)--(x2,y2)边上
				{
					xls[ptn] = x3;
					yls[ptn] = y3;
					dhz[ptn] = q->marker;
					ptn++;
				}
				else if (bFu3 && (!bF2) && (!bF3) && j != areaClip->node_list.length() - 1)//(x1,y1)点在(x3,y3)--(x4,y4)边上//length()-2可能有问题
				{
					xxls[dds] = x1;
					yyls[dds] = y1;
					jdwz[dds] = j;
					markerList[dds] = p->marker;
					xa = (x1 + 0.002 * x2) / (1.002);
					ya = (y1 + 0.002 * y2) / (1.002);
					bz = IsInRegion(xa, ya, areaClip);
					if (bz)
						q->flag = clipbz1;//入点
					else
						q->flag = clipbz2;//出点
					p->flag = q->flag;
					dds++;
				}
			}
			else if (CalLineJoint(x1, y1, x2, y2, x3, y3, x4, y4, xx, yy)) {
				xls[ptn] = xx[0];
				yls[ptn] = yy[0];
				xxls[dds] = xx[0];
				yyls[dds] = yy[0];
				jdwz[dds] = j;
				markerList[dds] = "J" + QString::number(dds + 1, 10);
				dhz[ptn] = "J" + QString::number(dds + 1, 10);
				ptn++;
				dds++;
			}
			q = q->next;
		}
		if (ptn > 0)//将交点插入被合并多边形链表中
		{
			if (ptn == 1)//交点只有一个，则直接插入链表
			{
				r1 = new ClipRegion;
				r1->x = xls[0];
				r1->y = yls[0];
				r1->marker = dhz[0];
				lamda = 0.002;
				xa = (xls[0] + lamda * x2) / (1.0 + lamda);
				ya = (yls[0] + lamda * y2) / (1.0 + lamda);
				bz = IsInRegion(xa, ya, areaClip);
				xa = (xls[0] + lamda * x1) / (1.0 + lamda);
				ya = (yls[0] + lamda * y1) / (1.0 + lamda);
				int bbbz = IsInRegion(xa, ya, areaClip);
				if (bbbz != bz)
					if (bz)
						r1->flag = clipbz1;//入点
					else
						r1->flag = clipbz2;//出点
				else
					r1->flag = 5;
				r1->next = p->next;
				p->next = r1;
				p = r1;
			}
			else //若交点大于一个，则对交点根据其与(x1,y1)点的距离，确定其顺序
			{
				for (k = 0; k < ptn; k++) //计算交点到(x1,y1)的距离
					djl[k] = qSqrt((xls[k] - x1)*(xls[k] - x1) + (yls[k] - y1)*(yls[k] - y1));
				for (k = 0; k < ptn - 1; k++)//交点按距离(x1,y1)点进行排序
				{
					int min = k;
					for (int l = k + 1; l < ptn; l++)
						if (djl[l] < djl[min])
							min = l;

					double temp = djl[k];
					djl[k] = djl[min];
					djl[min] = temp;

					temp = xls[k];
					xls[k] = xls[min];
					xls[min] = temp;
					temp = yls[k];
					yls[k] = yls[min];
					yls[min] = temp;

					QString dlss = dhz[k];
					dhz[k] = dhz[min];
					dhz[min] = dlss;
				}
				for (k = 0; k < ptn; k++)//将交点插入链表中
				{
					r1 = new ClipRegion;
					r1->x = xls[k];
					r1->y = yls[k];
					r1->marker = dhz[k];
					lamda = 0.002;
					xa = (xls[k] + lamda * x2) / (1.0 + lamda);
					ya = (yls[k] + lamda * y2) / (1.0 + lamda);
					bz = IsInRegion(xa, ya, areaClip);
					xa = (xls[k] + lamda * x1) / (1.0 + lamda);
					ya = (yls[k] + lamda * y1) / (1.0 + lamda);
					int bbbz = IsInRegion(xa, ya, areaClip);
					if (bbbz != bz)
						if (bz)
							r1->flag = clipbz1; //入点
						else
							r1->flag = clipbz2;//出点
					else
						r1->flag = 5;
					r1->next = p->next;
					p->next = r1;
					p = r1;
				}
			}
		}
		p = p->next;
	}

	//对合并多边形区域点按顺序排列
	q = &Reg2;
	for (k = 0; k < dds; k++)//交点按边的顺序进行排序
	{
		int min = k;
		for (int l = k + 1; l < dds; l++)
			if (jdwz[l] < jdwz[min])
				min = l;
		int temp = jdwz[k];
		jdwz[k] = jdwz[min];
		jdwz[min] = temp;

		QString cs = markerList[k];
		markerList[k] = markerList[min];
		markerList[min] = cs;

		double temp1 = xxls[k];
		xxls[k] = xxls[min];
		xxls[min] = temp1;
		temp1 = yyls[k];
		yyls[k] = yyls[min];
		yyls[min] = temp1;
	}

	int lsjds, jdqd;
	for (i = 0; i < areaClip->node_list.length(); i++)//对合并多边形每一边上有无交点进行判断，若有交点则进行插入链表中
	{
		x1 = q->x;
		y1 = q->y;
		lsjds = 0;
		jdqd = 0;
		for (j = 0; j < dds; j++)
		{
			if (jdwz[j] == i)
			{
				if (lsjds == 0)
					jdqd = j;
				lsjds++;
			}
		}
		if (lsjds > 0)//有交点
		{
			if (lsjds == 1)//1个交点直接插入
			{
				r2 = new ClipRegion;
				r2->x = xxls[jdqd];
				r2->y = yyls[jdqd];
				r2->marker = markerList[jdqd];
				r2->flag = -100;
				r2->next = q->next;
				q->next = r2;
				q = r2;
			}
			else//大于一个交点
			{
				for (k = 0; k < lsjds; k++)//计算交点到(x1,y1)的距离
				{
					djl[k] = qSqrt((xxls[jdqd + k] - x1)*(xxls[jdqd + k] - x1) + (yyls[jdqd + k] - y1)*(yyls[jdqd + k] - y1));
				}
				for (k = 0; k < lsjds - 1; k++)//交点按距离(x1,y1)点进行排序
				{
					int min = k;
					for (int l = k + 1; l < lsjds; l++)
					{
						if (djl[l] < djl[min])
							min = l;
					}
					double temp = djl[k];
					djl[k] = djl[min];
					djl[min] = temp;

					temp = xxls[jdqd + k];
					xxls[jdqd + k] = xxls[jdqd + min];
					xxls[jdqd + min] = temp;
					temp = yyls[jdqd + k];
					yyls[jdqd + k] = yyls[jdqd + min];
					yyls[jdqd + min] = temp;
					QString cs = markerList[jdqd + k];
					markerList[jdqd + k] = markerList[jdqd + min];
					markerList[jdqd + min] = cs;
				}
				for (k = 0; k < lsjds; k++)
				{
					r2 = new ClipRegion;
					r2->x = xxls[jdqd + k];
					r2->y = yyls[jdqd + k];
					r2->marker = markerList[jdqd + k];
					r2->flag = -100;
					r2->next = q->next;
					q->next = r2;
					q = r2;
				}
			}
		}
		q = q->next;
	}

	//对交点是“入”、“出”点标记
	q = &Reg2;
	while (q->next != 0)
	{
		if (q->flag == -100)   // if the flag = -100, then the node is a intersection node
		{
			p = &Reg1;
			while (QString::compare(q->marker, p->marker) != 0 && p->next != 0)//与被合并多边形交点为同一点，则标记为同一标志值
				p = p->next;
			q->flag = p->flag;
		}
		q = q->next;
	}
	q = &Reg2;
	while (q->next != 0) //与被合并多边形同一点名者，标记为同一“入”、“出”点标志
	{
		p = &Reg1;
		while (QString::compare(q->marker, p->marker) != 0 && p->next != 0)
			p = p->next;
		q->flag = p->flag;
		q = q->next;
	}

	p = &Reg1;
	QString staDh = p->marker;
	int flbz = p->flag;
	while (p != nullptr) //将被合并多边形的首末点标记为同一点名、同一“入”、“出”点标记//nullptr
	{
		p = p->next;
		if (p != nullptr && p->next == nullptr)
		{
			p->marker = staDh;
			p->flag = flbz;
		}

	}
	p = &Reg2;
	staDh = p->marker;
	flbz = p->flag;
	while (p != nullptr)//将合并多边形的首末点标记为同一点名、同一“入”、“出”点标记//nullptr
	{
		p = p->next;
		if (p != nullptr && p->next == nullptr)
		{
			p->marker = staDh;
			p->flag = flbz;
		}
	}

	//将链表首尾相连
	int pt1, pt2, runbz = 1;//pt1+1=被合并多边形上点的个数,包括交点和多边形顶点;pt2+1=合并多边形上点的个数,包括交点和多边形顶点
	pt1 = 0;
	p = &Reg1;
	while (p->next->next != 0)
	{
		pt1++;
		p = p->next;
	}
	p->next = &Reg1;
	pt2 = 0;
	q = &Reg2;
	while (q->next->next != 0)
	{
		pt2++;
		q = q->next;
	}
	q->next = &Reg2;

	clipResuAreaNum = 0;//区域数
	int ppz = 1, jlqds = 0;
	while (runbz)
	{
		runbz = 0;
		p = &Reg1;
		ptn = 0;
		for (int lsi = 0; lsi <= pt1; lsi++)//<pt1改
		{
			q = p;
			if (p->flag == clipbz1)
			{
				runbz = 1;
				jlqds = lsi;
				lsi = pt1 + 5;
			}
			p = p->next;
		}
		if (runbz)
		{
			while (q != 0)
			{
				if (q->flag == clipbz1)//"入"点，沿着被合并多边形搜索顶点序列
				{
					ppz = 1;
					xxls[ptn] = q->x;
					yyls[ptn] = q->y;
					markerList[ptn] = q->marker;
					q->flag = 0;
					ptn++;
					s2 = &Reg2;
					for (int lsi = 0; lsi <= pt2; lsi++)
					{
						if (QString::compare(s2->marker, q->marker) == 0)//标记相同点名为相同的标志
						{
							s2->flag = 0;
							lsi = pt2 + 5;
						}
						s2 = s2->next;
					}
					if (QString::compare(s2->marker, q->marker) == 0)
						s2 = s2->next;
					while (s2 != 0)
					{
						if (s2->flag == clipbz2)
						{
							r1 = &Reg1;
							while (QString::compare(s2->marker, r1->marker) != 0)
								r1 = r1->next;
							ppz = 2;
							q = r1;
							s2 = 0;
						}
						else
						{
							xxls[ptn] = s2->x;
							yyls[ptn] = s2->y;
							markerList[ptn] = s2->marker;
							s2->flag = 0;
							ptn++;

							if (QString::compare(markerList[0], markerList[ptn - 1]) == 0)//首末点点名相同时裁剪出一个区域
							{
								s2 = 0;
							}
							else
							{
								r1 = &Reg1;
								for (int lsi = 0; lsi <= pt1; lsi++)
								{
									if (QString::compare(s2->marker, r1->marker) == 0)//标记相同点名为相同的标志
									{
										r1->flag = 0;
										lsi = pt1 + 5;
									}
									r1 = r1->next;
								}
								s2 = s2->next;
							}
						}
					}

				}
				else if (q->flag == clipbz2) //"出"点,沿着裁剪多边形搜索顶点序列
				{
					ppz = 2;
					xxls[ptn] = q->x;
					yyls[ptn] = q->y;
					markerList[ptn] = q->marker;
					q->flag = 0;
					ptn++;
					s2 = &Reg2;
					for (int lsi = 0; lsi <= pt2; lsi++)
					{
						if (QString::compare(s2->marker, q->marker) == 0)
							s2->flag = 0;
						s2 = s2->next;
					}
					q = q->next;
				}
				else
				{
					if (ppz == 2)//前方距离最近的交点为"出"点
					{
						xxls[ptn] = q->x;
						yyls[ptn] = q->y;
						markerList[ptn] = q->marker;
						q->flag = 0;
						ptn++;
						s2 = &Reg2;
						for (int lsi = 0; lsi <= pt2; lsi++)
						{
							if (QString::compare(s2->marker, q->marker) == 0)//标记相同点名为相同的标志
								s2->flag = 0;
							s2 = s2->next;
						}
						q = q->next;
					}
					else//前一个点不是"入"点
					{
						xxls[ptn] = q->x;
						yyls[ptn] = q->y;
						markerList[ptn] = q->marker;
						q->flag = 0;
						ptn++;
						q = q->next;
					}
				}
				if (ptn > 1)
				{
					if (QString::compare(markerList[0], markerList[ptn - 1]) == 0)//首末点点名相同时裁剪出一个区域
					{
						for (i = 0; i < ptn - 1; i++)
						{
							QPointF point = QPointF(xxls[i], yyls[i]);
							pointResultList << point;
						}
						pointResultList << QPointF(-1.0, -1.0);//芮桂华2013-2-27修改
						clipResuAreaNum++;
						q = 0;
					}
				}
			}
		}
	}

	PListToNormalize(pointResultList);

	return pointResultList;
}

double EditableGraphicsScene::CalArea(ShapeItem* area)
/* 本函数应该是用来计算多边形的面积，公式参见http://mathworld.wolfram.com/PolygonArea.html */
{
	double areaClockFlag = 0.0;
	int n = area->node_list.length();
	if (fabs(area->node_list.value(n - 1)->scenePos().x() -
		area->node_list.value(0)->scenePos().x()) < 0.000001 &&
		fabs(area->node_list.value(n - 1)->scenePos().y() -
			area->node_list.value(0)->scenePos().y()) < 0.00001)//首尾点相连
		n = n - 1;

	for (int i = 0; i < n; i++)
	{
		areaClockFlag +=
			0.5 * (area->node_list.value(i)->scenePos().x() *
				area->node_list.value((i + 1) % n)->scenePos().y() -
				area->node_list.value((i + 1) % n)->scenePos().x() *
				area->node_list.value(i)->scenePos().y());
	}

	return areaClockFlag;//逆时针area>0.0,顺时针area<0.0
}

void EditableGraphicsScene::ConvertAreaNodeSort(ShapeItem* area)
/* 本函数应该是将 Area 中 node_list 的顺序置反 */
{
	int count = area->node_list.length();
	QList<QPointF> newNodelist;

	for (int i = 0; i < count; i++)
	{
		QPointF p = area->node_list.value(count - 1 - i)->scenePos();
		newNodelist << p;
	}

	for (int i = 0; i < count; i++)
		area->node_list.value(i)->setPos(newNodelist.value(i));
}

bool EditableGraphicsScene::IsInRegion(double x, double y, ShapeItem* area)
/* 本函数应该是判断点 (x,y) 是否在 area 里面 */
/* 本函数应该错误，没有考虑(-1,-1)点的情况 */
{
	double x1, y1, x2, y2, xpt, ypt;
	double minx, maxx, miny, maxy;
	int n = area->node_list.length();

	if (n == 0) return false;

	int i;
	minx = maxx = area->node_list.value(0)->scenePos().x();
	miny = maxy = area->node_list.value(0)->scenePos().y();
	for (i = 1; i < n; i++)
	{
		if (area->node_list.value(i)->scenePos().x() > 0 &&
			area->node_list.value(i)->scenePos().x() > 0 &&
			area->node_list.value(i)->scenePos().y() > 0 &&
			area->node_list.value(i)->scenePos().y() > 0)
		{
			minx = minx < area->node_list.value(i)->scenePos().x() ?
				minx : area->node_list.value(i)->scenePos().x();
			maxx = maxx > area->node_list.value(i)->scenePos().x() ?
				maxx : area->node_list.value(i)->scenePos().x();
			miny = miny < area->node_list.value(i)->scenePos().y() ?
				miny : area->node_list.value(i)->scenePos().y();
			maxy = maxy > area->node_list.value(i)->scenePos().y() ?
				maxy : area->node_list.value(i)->scenePos().y();
		}
	}

	if (!(x >= minx - 0.000001 && x <= maxx + 0.000001 &&
		y >= miny - 0.000001 && y <= maxy + 0.000001))//点不在区域内
		return FALSE;

	//交点计数检验点是否在区域内
	xpt = maxx + 0.5 * (maxx - minx);//-x;//从点(x,y)作延长线到(xpt,ypt)
	ypt = y;

	QPolygonF poly = area->polygon();
	QPointF referencePos = area->pos();
	QVector<QPolygonF>	_poly;

	//计算直线(x,y)--(xpt,ypt)与line线的交点
	double xx1[8], yy1[8];
	int ds = 0;
	for (i = 0; i < n - 1; i++)
	{
		x1 = area->node_list.value(i)->scenePos().x();
		y1 = area->node_list.value(i)->scenePos().y();
		x2 = area->node_list.value(i + 1)->scenePos().x();
		y2 = area->node_list.value(i + 1)->scenePos().y();
		if (IsInLine(x, y, x1, y1, x2, y2))//点在线段i,i+1上
			return TRUE;
		else if (CalLineJoint(x, y, xpt, ypt, x1, y1, x2, y2, xx1, yy1))//直线(x,y)--(xpt,ypt)与line线的i,i+1线段有交点
		{
			if ((fabs(xx1[0] - x2) < 0.00001 && fabs(yy1[0] - y2) < 0.00001) ||
				(fabs(xx1[0] - x1) < 0.00001 && fabs(yy1[0] - y1) < 0.00001))
				if (yy1[0] > (y1 + y2)*0.5)
					ds++;
				else
					ds = ds;
			else
				ds++;
		}
		else
			ds = ds;
	}

	x1 = area->node_list.value(n - 1)->scenePos().x();
	y1 = area->node_list.value(n - 1)->scenePos().y();
	x2 = area->node_list.value(0)->scenePos().x();
	y2 = area->node_list.value(0)->scenePos().y();
	if (IsInLine(x, y, x1, y1, x2, y2))//点在线段i,i+1上
		return TRUE;
	else if (CalLineJoint(x, y, xpt, ypt, x1, y1, x2, y2, xx1, yy1))//直线(x,y)--(xpt,ypt)与line线的i,i+1线段有交点
	{
		if ((fabs(xx1[0] - x2) < 0.00001 && fabs(yy1[0] - y2) < 0.00001) ||
			(fabs(xx1[0] - x1) < 0.00001 && fabs(yy1[0] - y1) < 0.00001)) {
			if (yy1[0] > (y1 + y2)*0.5)
				ds++;
			else
				ds = ds;
		}
		else
			ds++;
	}
	else
		ds = ds;

	if (ds % 2 == 0)//交点为偶数，则点(x,y)不在区域内
		return FALSE;
	else if (ds % 2 == 1)//交点为奇数，则点(x,y)在区域内
		return TRUE;
	else
		return FALSE;

}

bool EditableGraphicsScene::IsInLine(double x, double y, double x1, double y1, double x2, double y2)
/* 判断点 (x,y) 是否在由 (x1,y1) 和 (x2,y2) 构成的直线上 */
{
	double f, dite;
	dite = x2 - x1;

	if (fabs(dite) < 0.00001)
		f = x - x1;
	else
		f = (y - y1) - (y2 - y1)*(x - x1) / dite;

	if (fabs(f) < 0.0001 &&
		x >= std::min(x1, x2) - 0.0000001 &&
		x <= std::max(x1, x2) + 0.0000001 &&
		y >= std::min(y1, y2) - 0.0000001 &&
		y <= std::max(y1, y2) + 0.0000001)
		return true;
	else
		return false;

}

bool EditableGraphicsScene::CalLineJoint(
	double x1, double y1,
	double x2, double y2,
	double x3, double y3,
	double x4, double y4,
	double *xx, double *yy)
	/* 计算两条直线的交点，构成第一条直线的点为 (x1,y1),(x2,y2)， 构成第二条直线的点为 (x3,y3),(x4,y4)，交点为 (xx,yy)。若没有交点，返回 false */
{
	double dit, t, rou, x, y;
	dit = (y2 - y1) * (x4 - x3) - (x2 - x1) * (y4 - y3);
	if (fabs(dit) < 0.0001)
		return FALSE; //AfxMessageBox("两直线平行!");
	else {
		t = ((y3 - y1) * (x4 - x3) - (x3 - x1) * (y4 - y3)) / dit;
		rou = ((x2 - x1) * (y3 - y1) - (x3 - x1) * (y2 - y1)) / dit;
		if (t >= 0.0 && t <= 1.0001 && rou >= 0.0 && rou <= 1.0001)
		{
			x = x1 + (x2 - x1) * t, y = y1 + (y2 - y1) * t;
			if ((x >= std::min(x1, x2) - 0.0001 &&
				x <= std::max(x1, x2) + 0.0001 &&
				y >= std::min(y1, y2) - 0.0001 &&
				y <= std::max(y1, y2) + 0.0001) &&
				(x >= std::min(x3, x4) - 0.0001 &&
					x <= std::max(x3, x4) + 0.0001 &&
					y >= std::min(y3, y4) - 0.0001 &&
					y <= std::max(y3, y4) + 0.0001))
			{
				//AfxMessageBox("直线有交点!");
				*xx = x;
				*yy = y;
				return TRUE;
			}
			else
			{
				//AfxMessageBox("交点不在直线须范围内!");
				return FALSE;
			}
		}
		else
		{
			//AfxMessageBox("直线没有交点!");
			return FALSE;
		}
	}
}

QList<QPointF> EditableGraphicsScene::NodeCrossRegion(ShapeItem* area, NodeItem* nodegrabbed)
/* 拖动node,与选区其他边线产生交点*/
{
	QList<QPointF> qList;
	ShapeItem* areaClipTri = new ShapeItem();
	for (int i = 0; i < area->node_list.length(); i++)
	{
		if (area->node_list.value(i) == nodegrabbed)
		{
			if (i == 0)  // 第一个点
			{
				areaClipTri->node_list <<
					area->node_list.value(area->node_list.length() - 1);
				areaClipTri->node_list << area->node_list.value(0);
				areaClipTri->node_list << area->node_list.value(1);
			}
			else if (i == area->node_list.length() - 1) // 最后一个点
			{
				areaClipTri->node_list <<
					area->node_list.value(area->node_list.length() - 2);
				areaClipTri->node_list <<
					area->node_list.value(area->node_list.length() - 1);
				areaClipTri->node_list << area->node_list.value(0);
			}
			else
			{
				areaClipTri->node_list << area->node_list.value(i - 1);
				areaClipTri->node_list << area->node_list.value(i);
				areaClipTri->node_list << area->node_list.value(i + 1);
			}
		}
	}
	qList = ClipAnyRegion(area, areaClipTri, 0);
	return qList;
}

int EditableGraphicsScene::ClipResultNum(QList<QPointF> qList)
/* 用来判断裁剪结果中有几个区域。不同区域之间用两个QPointF(0,0)来间隔 *//* 为什么本函数并未被调用！*/
{
	int resultNum = 0;
	for (int i = 0; i < qList.length(); i++)
	{
		if (qList.value(i) == QPointF(0, 0) &&
			qList.value(i + 1) == QPointF(0, 0))
		{
			resultNum++;
			i++;
		}
	}
	return resultNum;
}

void EditableGraphicsScene::AddRegionPress(ShapeItem* _area)
/* 应该是按下鼠标的时候添加区域 */
{
	ShapeItem* areaAdd = _area;

	for (int i = 0; i < m_shapes.count(); i++)
	{
		if (i == m_selectedregions) //排除掉自己和自己合并
			continue;

		PolygonMerger mergeCmd(this, m_shapes.value(i), _area);
		mergeCmd.merge();
		if (mergeCmd.IsMerged()) // 如果融合了
		{
			_area = mergeCmd.GetMergedArea();
			i--;
		}
		else // 如果没有融合
		{
			if (m_shapes.last()->node_list.length() < 3)  continue;

			if (i != m_shapes.count() - 1) {
				if (AreaContainArea(m_shapes.last(),
					m_shapes.value(i)))
				{ // 如果 m_shapes.last() 包含 m_shapes.value(area_order)，就删除 m_shapes.value(area_order)				
					DelArea(m_shapes.value(i));
					m_shapes.removeOne(m_shapes.value(i));
					i--;
					break;
				}
				else if (AreaContainArea(m_shapes.value(i),
					m_shapes.last()) && !m_shapes.isEmpty()) { // 如果 m_shapes.value(area_order) 包含 m_shapes.last()，就删除 m_shapes.last()				
					DelArea(m_shapes.last());
					m_shapes.removeOne(m_shapes.last());
				}
			}
		}

		_area->SetNodesVisible(true);
		_area->transparency_count = 100;
		_area->pen_style = false;
	}
}

void EditableGraphicsScene::AddRegionMove(ShapeItem* _area)
/* 应该是移动鼠标的时候添加区域 */
{
	for (int area_order = 0; area_order < m_shapes.length();
		area_order++)//遍历比较m_shapes中所有区域
	{
		if (area_order == m_selectedregions)   // 这里与前一个函数这部分写得不同
			continue;

		PolygonMerger mergeCmd(this,
			m_shapes.value(area_order), _area);
		mergeCmd.MergeMove();
		if (mergeCmd.IsMerged()) {
			bool IsContain = AreaContainArea(_area,
				m_shapes.value(area_order));
			if (IsContain)  // 如果 _area 包含 m_shapes.value(area_order)，就令 m_shapes.value(area_order) 不可见
			{
				m_shapes.value(area_order)->setVisible(false);
				m_shapes.value(area_order)->SetNodesVisible(true);
			}

			if (m_temp_shape != nullptr) {
				int num = m_temp_shape->node_list.length();
				for (int i = 0; i < num; i++)
					removeItem(m_temp_shape->node_list.value(i));
				removeItem(m_temp_shape);
				m_temp_shape = nullptr;
			}
			m_temp_shape = mergeCmd.GetMergedArea();
			_area = m_temp_shape;
		}
		else
		{
			if (m_shapes.last()->node_list.length() < 3)  continue;
			if (area_order == m_shapes.length() - 1) {
				_area->SetNodesVisible(true);
				_area->transparency_count = 100;
				_area->pen_style = false;
				continue;
			}

			bool IsContain = AreaContainArea(m_shapes.last(),
				m_shapes.value(area_order));
			if (IsContain)  // 如果 m_shapes.last() 包含 m_shapes.value(area_order)，就令 m_shapes.value(area_order) 不可见
			{
				m_shapes.value(area_order)->setVisible(false);
				m_shapes.value(area_order)->SetNodesVisible(false);
			}

			bool IsContain2 = AreaContainArea(
				m_shapes.value(area_order), m_shapes.last());
			if (IsContain2)  // 如果 m_shapes.value(area_order) 包含 m_shapes.last()，就令 m_shapes.last() 不可见
			{
				m_shapes.last()->SetNodesVisible(false);
				m_shapes.last()->pen_style = true;
				m_shapes.last()->transparency_count = 0;
				return;
			}

		}
		_area->SetNodesVisible(true);
		_area->transparency_count = 100;
		_area->pen_style = false;
	}
}

void EditableGraphicsScene::ClipRegionPress(ShapeItem* _needCutArea)
/* 应该是按下鼠标的时候裁剪区域 */
{
	int areaCount = m_shapes.length() - 1; //剪1的作用是_needCutArea是最后一个不能跟自己剪
	for (int area_order = 0; area_order < areaCount; area_order++)
	{
		if (m_shapes.value(area_order)->item_select)
		{
			PolygonCutCommand clipCmd(this,
				m_shapes.value(area_order), _needCutArea);
			clipCmd.ClipDo();

			if (clipCmd.IsCuted()) // 如果发生了裁剪
			{
				area_order--;
				areaCount--;
			}

			if (!clipCmd.IsCuted()) // 如果没有裁剪
			{
				if (_needCutArea->node_list.length() < 3)
					continue;

				bool IsContain = AreaContainArea(_needCutArea,
					m_shapes.value(area_order));

				if (IsContain)  // 如果裁剪区域 _needCutArea 包含了 m_shapes.value(area_order)，删除 m_shapes.value(area_order)
				{
					DelArea(m_shapes.value(area_order));
					m_shapes.removeOne(m_shapes.value(area_order));
					area_order--;
					areaCount--;
				}
			}
		}
	}
}

void EditableGraphicsScene::ClipRegionMove(ShapeItem* _area)
/* 应该是移动鼠标的时候裁剪区域 */
{
	for (int area_order = 0; area_order < m_shapes.length() - 1;
		area_order++) //遍历比较m_shapes中所有区域
	{
		if (m_shapes.value(area_order)->item_select)
		{
			PolygonCutCommand mergeCmd(this,
				m_shapes.value(area_order), _area);
			mergeCmd.ClipMove();
			if (mergeCmd.IsCuted())
				m_temp_shapes << mergeCmd.GetMergedAreaArray();//获取当前剪切分成的区域列表放如m_temp_shapes
			else
			{
				if (m_shapes.last()->node_list.length() < 3 ||
					area_order == m_shapes.length() - 1)
					continue;
				bool IsContain = AreaContainArea(m_shapes.last(),
					m_shapes.value(area_order));
				if (IsContain)
				{
					m_shapes.value(area_order)->setVisible(false);
					m_shapes.value(area_order)->SetNodesVisible(false);
					m_shapes.value(area_order)->item_select = true;
					_area->pen_style = false;
				}
			}
		}
	}
}

void EditableGraphicsScene::DelArea(ShapeItem* _area)
{
	if (_area != nullptr)
	{
		int num = _area->node_list.length();
		/* 删除区域的节点 node */
		for (int i = 0; i < num; i++)
		{
			removeItem(_area->node_list.value(i));
		}
		removeItem(_area); // 删除区域
		_area = nullptr;
	}
}

void EditableGraphicsScene::AddInitSet()//合并
{
	for (int i = 0; i < m_shapes.length() - 1; i++) //将m_shapes中所有区域全部显示
	{
		m_shapes.value(i)->setVisible(true);
	}
	if (m_temp_shape != nullptr) // 若 m_temp_shape 不为空，则删除 m_temp_shape 及其节点
	{
		int num = m_temp_shape->node_list.length();
		for (int i = 0; i < num; i++)
		{
			removeItem(m_temp_shape->node_list.value(i));
		}
		removeItem(m_temp_shape);
		m_temp_shape = nullptr;
	}

}

void EditableGraphicsScene::ClipInitSet()//剪裁
{
	//show all regions in m_shapes
	for (int i = 0; i < m_shapes.length() - 1; i++)
	{
		m_shapes.value(i)->setVisible(true);
	}
	//clear
	if (m_temp_shapes.isEmpty()) return;

	for (int i = 0; i < m_temp_shapes.count(); i++)
	{
		int num = m_temp_shapes[i]->node_list.length();

		for (int k = 0; k < num; k++)
		{
			removeItem(m_temp_shapes[i]->node_list.value(k));
		}
		removeItem(m_temp_shapes[i]);
	}
	m_temp_shapes.clear();

}

void EditableGraphicsScene::AreaLastSel()
/*应该是设置有且只有 m_shapes.last() 被选中，其节点可见 */
{
	for (int i = 0; i < m_shapes.length(); i++)
	{
		m_shapes.value(i)->setSelected(false);
		m_shapes.value(i)->SetNodesVisible(false);
		m_shapes.value(i)->item_select = false;
	}
	m_shapes.last()->setSelected(true);
	m_shapes.last()->SetNodesVisible(true);
	m_shapes.last()->item_select = true;
}

void EditableGraphicsScene::DrawExternalArea(QList<QPointF> pList)
/* 应该是显示从外部导入的区域 ,并未发现本函数被调用 */
{
	while (m_shapes.length())  // 删除已经存在的所有区域
	{
		DelArea(m_shapes.last());
		m_shapes.removeOne(m_shapes.last());
	}

	if (!pList.isEmpty())
	{
		ShapeItem* area_new = new ShapeItem();
		addItem(area_new);
		QPointF pos_refer(0.0, 0.0);
		QPointF point_state(0.0, 0.0);
		QPolygonF poly = area_new->polygon();
		for (int i = 0; i < pList.length(); i++)  //找出所有合并后的点
		{
			point_state.setX(pList.value(i).x());
			point_state.setY(pList.value(i).y());
			if (point_state.x() && point_state.y())  // 如果 point_state 不是 (0,0) 点，则添加新的节点
			{
				NodeItem* node = new NodeItem(area_new);
				node->setPos(point_state);
				addItem(node);
				area_new->node_list << node;
				pos_refer = area_new->node_list.value(0)->scenePos();
				poly << node->scenePos() - pos_refer;
			}
		}
		area_new->setPos(pos_refer);
		area_new->setPolygon(poly);
		m_shapes << area_new;
		area_new->item_select = true;
		area_new->setSelected(false);
	}
}

void EditableGraphicsScene::DrawExternalAreas(QList<QList<QPointF>>  _pList)
/*应该是显示从外部导入的多个区域 */
{
	while (m_shapes.length())   // 删除已经存在的区域
	{
		DelArea(m_shapes.last());
		m_shapes.removeOne(m_shapes.last());
	}

	for (int num = 0; num < _pList.count(); num++)
	{
		QList<QPointF> pList = _pList.value(num);
		if (!pList.isEmpty())
		{
			bool _isCut = false;    // _isCut 用来判断有没有开始裁剪
			ShapeItem* area_new = new ShapeItem();
			ShapeItem* area_cut = new ShapeItem();
			addItem(area_new);
			QPointF pos_refer(0.0, 0.0);

			QPointF point_state(0.0, 0.0);
			QPointF pos_cut_refer(0.0, 0.0);

			QPolygonF poly = area_new->polygon();
			QPolygonF poly_cut = area_cut->polygon();

			for (int i = 0; i < pList.length(); i++)//////找出所有合并后的点
			{
				point_state.setX(pList.value(i).x());
				point_state.setY(pList.value(i).y());

				if (!_isCut) { // 如果还没有开始裁剪，那么节点都是外部的
					if (point_state.x() == -1) {  // 如果遇到了(-1,-1)，那么说明后面的都是需要裁剪的部分
						area_new->setPos(pos_refer);
						area_new->setPolygon(poly);
						m_shapes << area_new;
						area_new->item_select = true;
						area_new->setSelected(true);

						_isCut = true;
						continue;
					}
					NodeItem* node = new NodeItem(area_new);
					node->setPos(point_state);
					addItem(node);
					area_new->node_list << node;
					pos_refer = area_new->node_list.value(0)->scenePos();
					poly << node->scenePos() - pos_refer;

					if (i == pList.length() - 1) { // 如果到了最后一个点，我们就做些结束工作
						area_new->setPos(pos_refer);
						area_new->setPolygon(poly);
						m_shapes << area_new;
						area_new->item_select = true;
						area_new->setSelected(true);
					}

				}
				else {
					if (point_state.x() == -1) {  // 如果又遇到了(-1,-1)，那么说明后面是一个新的裁剪多边形
						m_shapes << area_cut;
						area_cut->setPos(pos_cut_refer);
						area_cut->setPolygon(poly_cut);
						for (int j = 0; j < area_cut->node_list.length(); j++)
							removeItem(area_cut->node_list.value(j));
						ClipRegionPress(area_cut);
						DelArea(area_cut);
						m_shapes.removeOne(area_cut);

						area_cut = new ShapeItem();
						poly_cut = area_cut->polygon();
						continue;
					}
					NodeItem* node = new NodeItem(area_cut);
					node->setPos(point_state);
					addItem(node);
					area_cut->node_list << node;
					pos_cut_refer = area_cut->node_list.value(0)->scenePos();
					poly_cut << node->scenePos() - pos_cut_refer;
					area_cut->setPos(pos_cut_refer);
					area_cut->setPolygon(poly_cut);
					if (i == pList.length() - 1) { // 如果到了最后一个点，我们就做些结束工作
						m_shapes << area_cut;
						area_cut->setPos(pos_cut_refer);
						area_cut->setPolygon(poly_cut);
						for (int j = 0; j < area_cut->node_list.length(); j++)
							removeItem(area_cut->node_list.value(j));
						ClipRegionPress(area_cut);
						DelArea(area_cut);
						m_shapes.removeOne(area_cut);
					}
				}
			}
		}
	}
}
/* save all selections into a QList<QList<QPointF>> list*/
QList<Rgn2D> EditableGraphicsScene::CollectAreas()
{
	QList<QList<QPointF>> pListArea;

	for (int i = 0; i < m_shapes.length(); ++i) {
		QList<QPointF> qlist;
		ShapeItem* _area = new ShapeItem();
		_area = m_shapes.value(i);

		for (int j = 0; j < _area->node_list.length(); ++j) {
			QPointF _point;
			_point.setX(_area->node_list.value(j)->x());
			_point.setY(_area->node_list.value(j)->y());
			qlist << _point;
		}
		pListArea << qlist;
	}
	return pListArea;
}

void EditableGraphicsScene::DrawExternalStartPoint(QPointF p)
/* 应该是显示外部导入的种子点 , 没有发现本函数被调用！ */
{
	while (m_seeds.length())  // 删除所有已经存在的种子点
	{
		removeItem(m_seeds.last());
		m_seeds.removeOne(m_seeds.last());
	}

	SeedItem* sp = new SeedItem;
	addItem(sp);
	sp->setPos(p);
	m_seeds << sp;
}

void EditableGraphicsScene::DrawExternalStartPoints(QList<QPointF> _pList)
/*应该是显示外部导入的所有种子点 */
{
	while (m_seeds.length())   // 删除所有已经存在的种子点
	{
		removeItem(m_seeds.last());
		m_seeds.removeOne(m_seeds.last());
	}

	for (int i = 0; i < _pList.length(); i++)
	{
		SeedItem* sp = new SeedItem;
		addItem(sp);
		sp->setPos(_pList.value(i));
		m_seeds << sp;
	}
}
void EditableGraphicsScene::ClearALLBGPoints()
{
	while (m_bgpoints.length())
	{
		removeItem(m_bgpoints.last());
		m_bgpoints.removeOne(m_bgpoints.last());
	}
}
void EditableGraphicsScene::ClearALLStartPoints()
/*应该是清除所有的种子点 *//* 为什么上面两个函数不调用这个函数？ */
{
	while (m_seeds.length())
	{
		removeItem(m_seeds.last());
		m_seeds.removeOne(m_seeds.last());
	}
	while (m_numbers.length())
	{
		removeItem(m_numbers.last());
		m_numbers.removeOne(m_numbers.last());
	}
	while (m_shapes.length())
	{
		removeItem(m_shapes.last());
		m_shapes.removeOne(m_shapes.last());
	}
}

QList<QPointF> EditableGraphicsScene::CollectStartPoints()
/* 将所有种子点的几何信息存储在一个 QList<QPointF> 里面 */
{
	QList<QPointF> _plist;

	for (int i = 0; i < m_seeds.length(); i++)
	{
		QPointF _point;
		_point.setX(m_seeds.value(i)->x());
		_point.setY(m_seeds.value(i)->y());
		_plist << _point;
	}
	return _plist;
}
//实现鼠标拖动节点，区域移动功能
void EditableGraphicsScene::setAreaToNode(QPointF point)
{
	NodeItem* node_grabbed = dynamic_cast<NodeItem*>(mouseGrabberItem());
	if (node_grabbed != 0)
	{
		int count = node_grabbed->parent_->node_list.length();
		QPolygonF poly_new;
		QPointF pos_refer = node_grabbed->parent_->node_list.value(0)->scenePos();
		for (int i = 0; i < count; i++)
		{
			NodeItem* node = node_grabbed->parent_->node_list.value(i);
			if ((node->scenePos().x() == -1.0) && (node->scenePos().y() == -1.0))
			{
				poly_new << QPointF(-1.0, -1.0);
				continue;
			}
			poly_new << node->scenePos() - pos_refer;
		}
		node_grabbed->parent_->setPos(pos_refer);
		node_grabbed->parent_->setPolygon(poly_new);
	}
}

ShapeItem* EditableGraphicsScene::CopyArea(ShapeItem* _area)//创建一个副本
{
	ShapeItem* area_temp = new ShapeItem();

	QPolygonF poly_temp = _area->polygon();
	QPointF	  pos_refer = _area->node_list.value(0)->scenePos();
	for (int i = 0; i < _area->node_list.length(); i++)
	{
		float x = _area->node_list.value(i)->x();
		float y = _area->node_list.value(i)->y();
		if (x != -1.0 || y != -1.0) {
			NodeItem* node = new NodeItem(area_temp);
			node->setPos(QPointF(x, y));
			area_temp->node_list << node;
			poly_temp << QPointF(x, y) - pos_refer;
		}
	}
	area_temp->setPos(pos_refer);
	area_temp->setPolygon(poly_temp);

	return area_temp;
}
ShapeItem* EditableGraphicsScene::cpyFrom(const ShapeItem* _other)
{
	ShapeItem* pRgn = new ShapeItem();
	NodeItem* pNode = new NodeItem(pRgn);
	auto plg = _other->polygon();
	auto pos_ref = _other->node_list.value(0)->scenePos();
	auto nodes = _other->node_list;
	std::for_each(nodes.begin(), nodes.end(),
		[&](const NodeItem* _pNode)
		{
			float x = _pNode->x(), y = _pNode->y();
			if (x != -1.f || y != -1.f)
			{
				pNode->set_parent(pRgn);
				pNode->setPos({ x, y });
				pRgn->node_list << pNode;
				plg << QPointF(x, y) - pos_ref;
			}
		});
	pRgn->setPos(pos_ref);
	pRgn->setPolygon(plg);

	return pRgn;
}

bool EditableGraphicsScene::IsContainArea(ShapeItem* _area1, ShapeItem* _area2)//_area1为包含区域，_area2为被包含区域
/* 应该是判断 _area1 是否包含 _area2 *//* this function is incorrect */
{
	QPointF   point_state(0.0, 0.0);
	QPolygonF  p1 = GetAreaToPolygon(_area1);

	/* 如果 _area2 的所有节点都在 _area1 内部, 那么 _area1 包含 _area2 */
	for (int i = 0; i < _area2->node_list.length(); i++) {
		if (!p1.containsPoint(_area2->node_list.value(i)->scenePos(),
			Qt::OddEvenFill))
			return false;
	}
	return true;
}

ShapeItem* EditableGraphicsScene::PListToArea(QList<QPointF> pList)//由点绘制1个区域
{
	if (pList.isEmpty())  return nullptr;

	ShapeItem* _area = new ShapeItem();
	addItem(_area);
	QPointF pTemp(0.0, 0.0);
	QPointF ref_point = pList.value(0);
	QPolygonF poly = _area->polygon();

	bool _areaKg = false;

	for (int i = 0; i < pList.length(); i++)
	{
		pTemp = pList.value(i);

		if (pTemp.x() == -1.0 && pTemp.y() == -1.0)
		{
			_areaKg = true;
			continue;
		}
		if (_areaKg)
		{
			_areaKg = false;
			poly << QPointF(-1.0, -1.0);

			NodeItem* node = new NodeItem(_area);
			node->setPos(-1.0, -1.0);
			_area->node_list << node;
		}

		NodeItem* node = new NodeItem(_area);
		node->setPos(pTemp);
		addItem(node);
		_area->node_list << node;
		poly << node->scenePos() - ref_point;
	}

	_area->setPos(ref_point);
	_area->setPolygon(poly);
	_area->setVisible(true);
	_area->SetNodesVisible(true);
	return _area;

}

ShapeItem* EditableGraphicsScene::PListToNoAddArea(QList<QPointF> pList)//由点绘制1个区域
/* 与上一个函数的区别貌似是不添加生成的 ShapeItem 到 Scene 中 */
{
	if (pList.isEmpty())  return nullptr;

	ShapeItem* _area = new ShapeItem();
	QPointF pTemp(0.0, 0.0);
	QPointF ref_point = pList.value(0);
	QPolygonF poly = _area->polygon();

	bool _areaKg = false;
	for (int i = 0; i < pList.length(); i++)
	{
		pTemp = pList.value(i);
		if (pTemp.x() == -1.0 && pTemp.y() == -1.0)
		{
			_areaKg = true;
			continue;
		}
		if (_areaKg)
		{
			_areaKg = false;
			poly << QPointF(-1.0, -1.0);

			NodeItem* node = new NodeItem(_area);
			node->setPos(-1.0, -1.0);
			_area->node_list << node;
		}

		NodeItem* node = new NodeItem(_area);
		node->setPos(pTemp);
		_area->node_list << node;
		poly << node->scenePos() - ref_point;
	}

	_area->setPos(ref_point);
	_area->setPolygon(poly);
	_area->setVisible(true);
	_area->SetNodesVisible(true);
	return _area;

}

QVector<ShapeItem*> EditableGraphicsScene::PListToMulitArea(QList<QPointF> pList)
/* 看来是将一个 pList 化为多个区域 */
{
	QVector<ShapeItem*> area_array;
	ShapeItem* _area = new ShapeItem();
	QPointF pTemp(0.0, 0.0);
	QPointF ref_point = pList.value(0);
	QPolygonF poly = _area->polygon();

	for (int i = 0; i < pList.length(); i++)
	{
		pTemp = pList.value(i);
		if (pTemp == QPointF(-1.0, -1.0))
		{
			_area->setPos(ref_point);
			_area->setPolygon(poly);
			area_array << _area;
			_area = new ShapeItem();
			poly.clear();
		}
		else
		{
			NodeItem* node = new NodeItem(_area);
			node->setPos(pTemp);
			_area->node_list << node;
			ref_point = _area->node_list.value(0)->scenePos();
			poly << node->scenePos() - ref_point;
		}
	}

	if (pList.last() != QPointF(-1.0, -1.0))
	{
		_area->setPos(ref_point);
		_area->setPolygon(poly);
		area_array << _area;
	}

	return area_array;
}

QVector<ShapeItem*> EditableGraphicsScene::AreaToMulitArea(ShapeItem* area)
{
	//针对区域多个空心，取多个空心区域
	QVector<ShapeItem*> area_array;
	QPolygonF poly;
	QPointF	  pos_refer;
	ShapeItem* tempArea = new ShapeItem();
	for (int i = 0; i < area->node_list.length(); i++)
	{
		float x = area->node_list.value(i)->x();
		float y = area->node_list.value(i)->y();
		if (x != -1.0 || y != -1.0)
		{
			NodeItem* node = new NodeItem(tempArea);
			node->setPos(QPointF(x, y));
			tempArea->node_list << node;
			pos_refer = tempArea->node_list.value(0)->scenePos();
			poly << QPointF(x, y) - pos_refer;
		}
		else if (x == -1.0 && y == -1.0)
		{
			tempArea->setPos(pos_refer);
			tempArea->setPolygon(poly);
			area_array << tempArea;
			tempArea = new ShapeItem();
			poly.clear();
		}
	}
	tempArea->setPos(pos_refer);
	tempArea->setPolygon(poly);
	area_array << tempArea;

	return area_array;
}
#pragma endregion

///------ get four corners of roi -------
void EditableGraphicsScene::GetLeftX(QPointF p, double& lp)
{
	lp = (double)p.x() < lp ? (double)p.x() : lp;
}
void EditableGraphicsScene::GetLeftY(QPointF p, double& lp)
{
	lp = p.y() < lp ? p.y() : lp;
}
void EditableGraphicsScene::GetRightX(QPointF p, double& lp)
{
	lp = p.x() > lp ? p.x() : lp;
}
void EditableGraphicsScene::GetRightY(QPointF p, double& lp)
{
	lp = p.y() > lp ? p.y() : lp;
}

void EditableGraphicsScene::update_point(size_t idx, float_t x, float_t y) noexcept {
	if (idx < m_seeds.size())
		m_seeds[idx]->setPos({ x, y });
	if (idx < m_numbers.size())
		m_numbers[idx]->setPos({ x, y });
}

QList<Rgn2D> EditableGraphicsScene::_Gather_selections()
{
	Rgn2D sltn;
	QList<Rgn2D> sltList;
	ShapeItem* pRgn = new ShapeItem();
	std::for_each(m_shapes.begin(), m_shapes.end(),
		[&](const ShapeItem* _pRI)
		{
			sltn.clear();
			pRgn = (ShapeItem*)_pRI;
			for (int i = 0; i < pRgn->node_list.length(); ++i)
			{
				QPointF node;
				node.setX(pRgn->node_list.value(i)->x());
				node.setY(pRgn->node_list.value(i)->y());
				sltn << node;
			}
			sltList << sltn;
		});
	return sltList;
}
QList<QPointF> EditableGraphicsScene::_Gather_points()
{
	QList<QPointF> pts;
	std::for_each(m_seeds.begin(), m_seeds.end(),
		[&pts](const SeedItem* _pPt)
		{
			QPointF _point;
			_point.setX(_pPt->x());
			_point.setY(_pPt->y());
			pts << _point;
		});
	std::for_each(m_bgpoints.begin(), m_bgpoints.end(),
		[&pts](auto pptr) {
			pts << QPointF{ pptr->x(), pptr->y() };
		});
	return pts;
}

QRectF EditableGraphicsScene::get_fit_view_rect()
{
	double leftx = width(), lefty = height(),
		rightx = 0, righty = 0;

	for (int i = 0; i < m_shapes.size(); i++)
	{
		for (int j = 0;
			j < m_shapes.value(i)->GetPolygon().size();
			j++)
		{
			QPointF point = m_shapes.value(i)->GetPolygon().value(j);
			if (point != QPointF(-1, -1))
			{
				GetLeftX(point, leftx);
				GetLeftY(point, lefty);
				GetRightX(point, rightx);
				GetRightY(point, righty);
			}
		}
	}

	if (m_shapes.size() == 0)
		return QRectF();
	else
		return QRectF(leftx, lefty, rightx - leftx, righty - lefty);
}
/* if there is no seed or area , return true */
bool EditableGraphicsScene::empty() const noexcept
{
	return m_shapes.isEmpty() && m_seeds.isEmpty();
}
/* check mouse position in which region */
ShapeItem* EditableGraphicsScene::cursor_captured_shape(QPointF pos)
{
	foreach(ShapeItem* area, m_shapes)
		if (area->PointInArea(area->mapFromScene(pos)))
			return area;

	return nullptr;
}
/* the pos in which seed */
SeedItem* EditableGraphicsScene::cursor_captured_seed(QPointF pos)
{
	QPointF posInItem;
	QTransform trans = this->views().value(0)->transform();
	double scalfac = trans.m11();

	foreach(SeedItem* seed, m_seeds)
	{
		posInItem = seed->mapFromScene(pos);
		if (seed->shape().contains(posInItem*scalfac))
			return seed;
	}
	return nullptr;
}
/* the pos in which node */
NodeItem* EditableGraphicsScene::cursor_captured_node(QPointF pos)
{
	QPointF posInItem;

	QTransform trans = this->views().value(0)->transform();
	double scalfac = trans.m11();

	foreach(ShapeItem* area, m_shapes) {
		foreach(NodeItem* node, area->node_list)
		{
			posInItem = node->mapFromScene(pos);
			if (node->boundingRect().contains(posInItem*scalfac))
				return node;
		}
	}

	return nullptr;
}
/* return region type which the mouse pos in */
int EditableGraphicsScene::posAreaType(QPointF pos)
// if is drawing, return -1;
// if there is no areas and nodes, return 0;
// if the pos is not in any areas or nodes, return 1;
// if the pos is in a area, return 2;
// if the pos is in a node return 3;
// if the pos is in a seed return 4;
{
	if (m_isdrawing)
		return -1;
	else if (empty())
		return 0;
	else if (cursor_captured_seed(pos) != nullptr)
		return 4;
	else if (cursor_captured_node(pos) != nullptr)
		return 3;
	else if (cursor_captured_shape(pos) != nullptr)
		return 2;
	else
		return 1;
}

int EditableGraphicsScene::rightButtonAreaType(QPointF* pos)
/* a interfact to wcf */
{
	*pos = m_eventpos;

	return posAreaType(m_eventpos);
}
/* check how many seeds in selected region*/
QSet<SeedItem*> EditableGraphicsScene::seedInArea(ShapeItem* area)
{
	QPointF seedPosition;
	QSet<SeedItem*> seedInAreaList;

	foreach(SeedItem* seed, m_seeds) {
		seedPosition = seed->scenePos();
		if (IsInRegion(seedPosition.x(), seedPosition.y(), area))
			seedInAreaList << seed;
	}

	return seedInAreaList;
}
///---------- create cursor label -------------
/*create cursor label to show the coordinates of mouse cursor*/
void EditableGraphicsScene::CreateCursorLabel()
{
	QSettings settings;
	QColor color = settings.value("roi_label_color",
		QColor(0, 0, 0)).value<QColor>();
	QFont font = settings.value("roi_label_font",
		QFont("msyh", 9)).value<QFont>();

	QTransform trans = views().value(0)->transform();
	double scalfac = trans.m11();

	m_horcursor_info = new AOICursorPositionLabel("");
	m_vercursor_info = new AOICursorPositionLabel("");

	m_horcursor_info->setDefaultTextColor(color);
	m_horcursor_info->setFont(QFont(font));
	m_horcursor_info->setFlags(QGraphicsItem::ItemIgnoresTransformations);
	m_horcursor_info->setPlainText(QString(" x : %1 ").arg(QString::
		number(m_last_pressedpos.x(), 'f', 2)));
	addItem(m_horcursor_info);
	m_horcursor_info->setVisible(true);
	m_horcursor_info->setZValue(1);

	m_vercursor_info->setDefaultTextColor(color);
	m_vercursor_info->setFont(QFont(font));
	m_vercursor_info->setFlags(QGraphicsItem::ItemIgnoresTransformations);
	m_vercursor_info->setPlainText(QString(" y : %1 ").arg(QString::
		number(m_last_pressedpos.y(), 'f', 2)));
	addItem(m_vercursor_info);
	m_vercursor_info->setVisible(true);
	m_vercursor_info->setZValue(1);

	QRectF rect = sceneRect();
	QPointF x_start = m_last_pressedpos +
		QPointF(15, 16) / scalfac;

	if (x_start.y() + m_horcursor_info->boundingRect().height() /
		scalfac > rect.bottom())
		x_start.setY(rect.bottom() -
			m_horcursor_info->boundingRect().height() / scalfac);
	if (x_start.x() + (m_horcursor_info->boundingRect().width() +
		3.0 + m_vercursor_info->boundingRect().width()) /
		scalfac > rect.right())
		x_start.setX(rect.right() -
		(m_horcursor_info->boundingRect().width() +
			3.0 + m_vercursor_info->boundingRect().width()) / scalfac);

	m_horcursor_info->setPos(x_start);

	QPointF y_start = m_horcursor_info->scenePos() +
		QPointF(m_horcursor_info->boundingRect().width() + 3, 0) / scalfac;
	m_vercursor_info->setPos(y_start);

	m_is_cursor_info_visible = true;
}
/*delete the coordinates label when the drawing complete*/
void EditableGraphicsScene::DeleteCursorLabel()
{
	if (m_is_cursor_info_visible) {
		m_is_cursor_info_visible = false;
		removeItem(m_horcursor_info);
		removeItem(m_vercursor_info);
		delete m_horcursor_info;
		delete m_vercursor_info;
	}

	//setBackgroundBrush(Qt::white);
}
/*show coordinates of cursor in label when mouse move*/
void EditableGraphicsScene::MoveCursorLabel(QPointF pos)
{
	QTransform trans = views().value(0)->transform();
	double scalfac = trans.m11();

	if (m_is_cursor_info_visible) {
		QRectF rect = sceneRect();

		QPointF x_start = pos + QPointF(15, 16) / scalfac;

		if (x_start.y() + m_horcursor_info->boundingRect().height() /
			scalfac > rect.bottom())
			x_start.setY(rect.bottom() -
				m_horcursor_info->boundingRect().height() / scalfac);
		if (x_start.x() + (m_horcursor_info->boundingRect().width() +
			3.0 + m_vercursor_info->boundingRect().width()) /
			scalfac > rect.right())
			x_start.setX(rect.right() -
			(m_horcursor_info->boundingRect().width() +
				3.0 + m_vercursor_info->boundingRect().width()) / scalfac);

		m_horcursor_info->setPos(x_start);
		m_horcursor_info->setPlainText(QString(" x : %1 ").arg(QString::
			number(pos.x(), 'f', 2)));

		QPointF y_start = m_horcursor_info->scenePos() +
			QPointF(m_horcursor_info->boundingRect().width() + 3, 0) / scalfac;
		m_vercursor_info->setPos(y_start);
		m_vercursor_info->setPlainText(QString(" y : %1 ").arg(QString::
			number(pos.y(), 'f', 2)));
	}
}
/*remove the last region in the region list*/
void EditableGraphicsScene::ClearLastArea()
{
	if (m_isdrawing) {
		removeItem(m_triangle_shape);

		DeleteCursorLabel();
		DelArea(m_shapes.last());
		m_shapes.removeOne(m_shapes.last());
		m_isdrawing = false;
		emit complete();
	}
}
/* check if one region contain another */
bool EditableGraphicsScene::AreaContainArea(ShapeItem* set, ShapeItem* subset)
/* judge whether set contain subset */
/* here we use QRegion, this method is slow, we need faster algorithm */
{
	QRegion setRegion = set->AreaToRegion();
	QRegion subsetRegion = subset->AreaToRegion();

	setRegion.translate(set->scenePos().toPoint());
	subsetRegion.translate(subset->scenePos().toPoint());

	return (setRegion + subsetRegion == setRegion) &&
		(setRegion - subsetRegion != setRegion) ? true : false;
}
/* check if two regions are intersecting */
bool EditableGraphicsScene::AreaIntersectArea(ShapeItem* set1, ShapeItem* set2)
/* here we use QRegion, this method is slow, we need faster algorithm */
{
	QRegion region1 = set1->AreaToRegion();
	QRegion region2 = set2->AreaToRegion();

	region1.translate(set1->scenePos().toPoint());
	region2.translate(set2->scenePos().toPoint());

	return region1.intersects(region2);
}
/* change region to a vector of ShapeItems */
QVector<ShapeItem*> EditableGraphicsScene::AreaToAreas(ShapeItem* area)
{
	QVector<ShapeItem*> areas;
	ShapeItem* areasListItem = new ShapeItem();
	QPolygonF areasListItemPolygon;

	if (area->node_list.length() == 0)
		return areas;

	QPointF pos_refer = area->node_list.value(0)->scenePos();
	for (int i = 0; i < area->node_list.length(); i++)
	{
		float x = area->node_list.value(i)->x();
		float y = area->node_list.value(i)->y();
		if (x != -1.0 || y != -1.0)
		{
			NodeItem* node = new NodeItem(areasListItem);
			node->setPos(QPointF(x, y));
			areasListItem->node_list << node;
			areasListItemPolygon << QPointF(x, y) - pos_refer;
		}
		else if (x == -1.0 && y == -1.0)
		{
			areasListItem->setPos(pos_refer);
			areasListItem->setPolygon(areasListItemPolygon);
			areas << areasListItem;
			areasListItem = new ShapeItem();
			areasListItemPolygon.clear();
		}
	}
	areasListItem->setPos(pos_refer);
	areasListItem->setPolygon(areasListItemPolygon);
	areas << areasListItem;

	return areas;
}

QList<QPointF> EditableGraphicsScene::AreaToPointList(ShapeItem* area)
/* change the area to a list of point */
/* the area should not contain inner areas */
{
	QList<QPointF> pointList;
	for (auto nodeIterator = area->node_list.begin();
		nodeIterator != area->node_list.end(); nodeIterator++) /* for each node of area */
		pointList << (*nodeIterator)->scenePos();

	return pointList;
}

QList<QPolygonF*> EditableGraphicsScene::PListToPolygons(QList<QPointF> pointList)
{
	QList<QPolygonF* > polygons;
	//polygons.clear();
	QPolygonF* polygon = new QPolygonF();
	QPointF point(0.0, 0.0);

	if (!pointList.isEmpty()) {
		for (int i = 0; i < pointList.length(); i++)
		{
			point = pointList.value(i);
			if (point == QPointF(-1.0, -1.0))
			{
				polygons << polygon;
				polygon = new QPolygonF();
			}
			else
			{
				(*polygon) << point;
			}
		}

		if (pointList.last() != QPointF(-1.0, -1.0))
			polygons << polygon;
	}

	return polygons;
}

void EditableGraphicsScene::PolygonToCorrectOrder(QList<QPolygonF*>& polygons)
{
	int index = polygons.count() - 1;
	for (int i = polygons.count() - 1; i > 0; i--)
	{
		bool isContain = true;;

		for (int j = 0; j < polygons[i - 1]->count(); j++)
		{
			if (!polygons[index]->containsPoint(
				polygons[i - 1]->value(j), Qt::OddEvenFill))
			{
				isContain = false;
				break;
			}
		}

		if (!isContain)
			index = i - 1;
	}

	if (index > 0)
		polygons.swap(0, index);
}

QList<QPointF> EditableGraphicsScene::PolygonsToPList(QList<QPolygonF*> polygons, bool isCross)
{
	QList<QPointF > pointList;

	for (int i = 0; i < polygons.count(); i++) {
		QPolygonF* polygon = polygons[i];
		for (int j = 0; j < polygon->count(); j++)
			pointList << polygon->value(j);
		pointList << QPointF(-1.0, -1.0);
	}

	if (!isCross && !pointList.isEmpty())
		pointList.removeLast();

	return pointList;
}

void EditableGraphicsScene::PListToNormalize(QList<QPointF >& pointList)
{
	if (!pointList.isEmpty()) {
		QList<QPolygonF* > polygons = PListToPolygons(pointList);
		PolygonToCorrectOrder(polygons);
		bool isCross = (pointList.last() == QPointF(-1.0, -1.0));
		pointList = PolygonsToPList(polygons, isCross);
	}
}

void EditableGraphicsScene::SlotPickSSSIGPoint()
{
	m_last_drawstatus = m_drawstatus;
	m_drawstatus = _MyStatus::AUTO_SEED;
}
