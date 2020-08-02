#pragma once

#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include <QStyleOptionGraphicsItem>
#include <QList>

class NodeItem;

class ShapeItem : public QGraphicsPolygonItem
{
public:
	ShapeItem();
	~ShapeItem();

	QVariant itemChange(GraphicsItemChange change, const QVariant& value);

	void SetNodesVisible(bool visible);
	void SetNodesSelect(QPointF _p1);

	bool InAreaRange(QPointF point);
	bool PointInArea(QPointF point);  

	QPolygonF GetPolygon();
	QList<QPolygonF> AreaToPolygons(); 
	int ConvexHullIndex(QList<QPolygonF>);
	QRegion AreaToRegion();

	//QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

public:
	int transp;
	int subsetsize;
	int  transparency_count;
	bool pen_style;
	bool item_select;
	bool is_animation;
	QPolygonF  _poly;
	QList<NodeItem*> node_list;
};