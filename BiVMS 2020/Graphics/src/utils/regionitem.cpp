
#include <QtGui>
#include "utils/shape_item.h"
#include "utils/nodeitem.h"
#include <QGraphicsPolygonItem>
#include <qgraphicsscene.h>
#include <QGraphicsView>

AreaItem::AreaItem() : QGraphicsPolygonItem()
{
	transparency_count =100;
	pen_style = false;
	item_select = false;
	is_animation = false;
	transp = 255;
	subsetsize = 30;

	setBrush(QBrush(QColor(255,0,0,transparency_count)));
	setPen(QColor(255,0,0,100));
	setZValue(2.0);
}

AreaItem::~AreaItem()
{

}

void AreaItem::SetNodesVisible(bool visible)
{
	for (int i = 0; i < node_list.length(); ++i)
		node_list.value(i)->setVisible(visible);

	setSelected(visible);
	item_select = visible;
}

void AreaItem::SetNodesSelect(QPointF _p1)
{
	for (int i = 0;i < node_list.length(); i++)
	{
		if (scene() == 0)	
			return;
		QGraphicsView* topView = scene()->views().value(0); 
		QPoint viewPoint = topView->mapFromScene(_p1);
		QPoint viewBoundPoint = topView->mapFromScene(node_list.value(i)->scenePos());

		if (viewBoundPoint.x() >= viewPoint.x() - 8 && viewBoundPoint.x() <= viewPoint.x() + 8 &&
			viewBoundPoint.y() >= viewPoint.y() - 8 && viewBoundPoint.y() <= viewPoint.y() + 8)
			node_list.value(i)->setSel(true);
		else
			node_list.value(i)->setSel(false);
	}
}
/*check if point in selection*/
bool AreaItem::InAreaRange(QPointF p)
{
	bool isIn = false;

	QPolygonF poly = GetPolygon();
	if (poly.containsPoint(QPointF(p.x(),p.y()),Qt::OddEvenFill))
		isIn = true;
	return isIn;
}

QPolygonF AreaItem::GetPolygon()
{
	QPolygonF  poly;
	for (int k=0;k < node_list.length();k++){
		QPointF point_state;
		point_state.setX(node_list.value(k)->x());
		point_state.setY(node_list.value(k)->y());
		poly << point_state;
	}
	return poly;
}

QVariant AreaItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
	return QGraphicsPolygonItem::itemChange(change,value);
}
/*draw ROI region and set the pen-style*/
void AreaItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	
	QSettings setting;
	QColor color =  setting.value("roi_line_color",7/*Qt::red*/).value<QColor>();
	int width=setting.value("roi_line_width",0.5).toInt();
	
	QTransform trans =scene()->views().value(0)->transform(); 
	double scaling_factor=trans.m11();
	
	if (pen_style){
		painter->setPen(QPen(Qt::darkGreen, width / scaling_factor, Qt::DotLine, Qt::RoundCap, Qt::RoundJoin));
		color = setting.value("roi_rgn_color", QColor(255, 100, 100)).value<QColor>();
		transparency_count=0;
	}
	else{
		painter->setPen(QPen(Qt::green, width / scaling_factor, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
		color = setting.value("roi_rgn_color", QColor(255, 100, 100)).value<QColor>();
		transparency_count = setting.value("roi_transparent", 100).toInt();
	}
	color.setAlpha(transparency_count);
	painter->setBrush(color);

	QPolygonF poly = polygon();

  	QVector<QPolygonF>	_poly;
 	QPainterPath _path;

	QPolygonF temp_poly;
	for(int i = 0; i < poly.toPolygon().count(); i++){
		QPointF pTemp = poly.value(i);
		if (pTemp.x() == -1.0 && pTemp.y() == -1.0){
			_poly << temp_poly;
			temp_poly.clear();
 			continue;
		}
		temp_poly << pTemp;
	}
	_poly << temp_poly;

	int _poly_index = 0;
	_path.addPolygon(_poly[_poly_index]);

	painter->save();
	painter->setBrush(Qt::NoBrush);
	painter->setPen(QPen(setting.value("aoi_line_color",7/*Qt::red*/).value<QColor>(),width/scaling_factor,Qt::DotLine,Qt::RoundCap,Qt::RoundJoin));

	for (int i = 0;i<_poly.count();i++){
		if (i!=_poly_index){
			QPainterPath _tempPath;
			_tempPath.addPolygon(_poly[i]);
			_path = _path.subtracted(_tempPath);
			_tempPath.closeSubpath();
			painter->drawPath(_tempPath);
		}
	}

	_path.closeSubpath();
	painter->restore();
	painter->drawPath(_path);

	if (is_animation)
	{
		painter->setPen(QPen(QBrush(QColor(255, 228, 196, transp)), 3.0 / scaling_factor));
		QRectF boundaryrect = poly.boundingRect();
		QPointF lefttop,leftbottom,righttop,rightbottom;
		lefttop = boundaryrect.topLeft();
		leftbottom = boundaryrect.bottomRight();
		righttop = boundaryrect.topRight();
		rightbottom = boundaryrect.bottomRight();

		QRegion region = QRegion(poly.toPolygon(),Qt::OddEvenFill);
		painter->setClipRegion(region);
		for (int i = lefttop.y(); i < rightbottom.y(); i += subsetsize)
			painter->drawLine(lefttop.x(), i, righttop.x(), i);
		for (int j = lefttop.x(); j < rightbottom.x(); j += subsetsize)
			painter->drawLine(j, lefttop.y(), j, leftbottom.y());
	}
}
/* change the area to a list of polygons. the outer polygon should be the first, there is still some questions */
QList<QPolygonF > AreaItem::AreaToPolygons()
{
	QList<QPolygonF > polygons;  // the result, a list of polygon

	QPolygonF areaPolygon = polygon();
	QPolygonF polygonListItem;

	foreach (QPointF point, areaPolygon)
		if (point == QPointF(-1.0,-1.0))    // QPoint(-1,-1) is the partition point
		{
			polygons << polygonListItem;    // here come to a partition point, add the temp polygon to the list
			polygonListItem.clear();

		} else
			polygonListItem << point;

	polygons << polygonListItem;
	return polygons;
}
/* change the area to region */
QRegion AreaItem::AreaToRegion()
{
	QList<QPolygonF > polygons = AreaToPolygons();

	QList<QPolygonF>::iterator polygonIterator = polygons.begin();
	QRegion mainRegion((*polygonIterator++).toPolygon()); // the main region will be cut

	while (polygonIterator != polygons.end()) 
	{ //  clip the inner polygons
		QRegion clipRegion((*polygonIterator++).toPolygon());
		mainRegion -= clipRegion;
	}

	return mainRegion;
}
/* judge whether a point is in area, if in, return true */
bool AreaItem::PointInArea( QPointF point )
{
	return AreaToRegion().contains(point.toPoint());
}
/* convex hull */
int AreaItem::ConvexHullIndex(QList<QPolygonF > polygons)
{
	int index = polygons.count()-1;

	for (int i = index; i > 0; i--)
		if (polygons[index].united(polygons[index-1]) != polygons[index])
			index = i - 1;

	return index;
}