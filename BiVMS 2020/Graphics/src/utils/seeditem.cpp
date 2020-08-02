#include <QtGui>
#include "utils/seeditem.h"

extern bool isSelectBGpoint;

SeedItem::SeedItem()
	: QGraphicsItem() {
	/*QPainterPath circle;
	circle.addEllipse(-8,-24,16,16);
	QPainterPath triangle;
	triangle.moveTo(-8,-16);
	triangle.lineTo(8,-16);
	triangle.lineTo(0,0);
	triangle.lineTo(-8,-16);
	m_image_path = circle + triangle;*/

	QPainterPath cross;
	cross.addRect({ -1, -8, 2, 16 });
	cross.addRect({ -8, -1, 16, 2 });
	m_image_path = cross;

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIgnoresTransformations,true);
	setZValue(2.0);
}

SeedItem::~SeedItem() { }

QRectF SeedItem::boundingRect() const {
	return QRectF(-8,-24,16,24);
}

void SeedItem::paint( QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget ) {
	    painter->setPen(QPen(QColor(0,0,0)));
	   painter->setBrush(QColor(0,255,0,200));
	    //painter->setBrush(QColor(255, 0, 0, 200));//red,green,blue
	    painter->drawPath(m_image_path);
	    painter->setBrush(QColor(255,255,255));
	   //painter->drawEllipse(-4,-20,8,8);
}

QPainterPath SeedItem::shape() const {
	return m_image_path;
}

BGpointItem::BGpointItem()
	: QGraphicsItem() {
	QPainterPath circle;
	circle.addEllipse(-5,-5,10,10);

	m_shape = circle;

	setFlag(QGraphicsItem::ItemIsMovable, true);
	setFlag(QGraphicsItem::ItemIgnoresTransformations, true);
	setZValue(2.0);
}
BGpointItem::~BGpointItem() { }

void BGpointItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
	painter->setPen(QPen(QColor(0, 0, 0)));
	painter->setBrush(QColor(255, 0, 0, 200));
	painter->drawPath(m_shape);
	painter->setBrush(QColor(255, 255, 255));
	painter->drawEllipse(-2,-2,4,4);
}

QPainterPath BGpointItem::shape() const {
	return m_shape;
}

QRectF BGpointItem::boundingRect() const {
	return QRectF(-8, -24, 16, 24);
}