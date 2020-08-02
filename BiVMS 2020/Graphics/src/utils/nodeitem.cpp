#include "utils/nodeitem.h"
#include <QtGui>
#include <QGraphicsScene>
#include <qgraphicsscene.h>

NodeItem::NodeItem(AreaItem* parent)
	: QGraphicsItem()
{
	parent_=parent;
	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIsSelectable, false);
	setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
//	setFlag(QGraphicsItem::ItemIsFocusable,true);
//	setAcceptedMouseButtons(Qt::LeftButton);
	dragOver =false;
//	setAcceptDrops(true);
}

NodeItem::~NodeItem()
{

}

void NodeItem::set_parent(const AreaItem* _parent)
{
	parent_ = (AreaItem*)_parent;
}

void NodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);

	//painter->setBrush(Qt::darkGray);
	//painter->drawEllipse(-12, -12, 30, 30);
	//painter->setPen(QPen(Qt::black, 1));
	//painter->setPen(QPen(Qt::white,1));
	painter->setPen(Qt::NoPen);
	painter->setBrush(dragOver ? 
		QBrush(QColor(255,0,0,255)) : 
		QBrush(QColor(255,255,255,255)));
	painter->drawRect(-3, -3, 6, 6);
	//painter->drawEllipse(QRectF(-3,-3,6,6));
}

void NodeItem::mousePressEvent(QGrSME *e)
{
	dragOver= false;
	update();
	QGraphicsItem::mousePressEvent(e);
}

void NodeItem::mouseMoveEvent(QGrSME *e)
{
	dragOver= false;
	update();

	if ((e->buttons() & Qt::LeftButton) && (flags() & ItemIsMovable)) 
	{
		QRectF scene_rect = scene()->sceneRect();
		double max_x = scene_rect.x()+scene_rect.width();
		double max_y = scene_rect.y()+scene_rect.height();
		QPointF point = e->scenePos();
		double x,y;
		if (point.x()<scene_rect.x())
		{
			x=scene_rect.x();
		}
		else if (point.x()>max_x)
		{
			x = max_x;
		}
		else
		{
			x = point.x();
		}

		if (point.y()<scene_rect.y())
		{
			y= scene_rect.y();
		} 
		else if (point.y() > max_y)
		{
			y = max_y;
		}
		else
			y = point.y();

		setPos(QPointF(x,y));
		e->ignore();
		return;
	}
	else
		e->ignore();

	QGraphicsItem::mouseMoveEvent(e);
}

void NodeItem::mouseReleaseEvent(QGrSME *event)
{
	//dragOver =false;
	//update();
// 	setFlag(QGraphicsItem::ItemIsMovable, false);

	QGraphicsItem::mouseReleaseEvent(event);
}
QRectF NodeItem::boundingRect() const
{
	   return QRectF(-8, -8, 16,16);
	   //return QRectF(0, 0, 0, 0);
}

 void NodeItem::setSel(bool _select)
 {
	 dragOver=_select;
 	 update();
 }
