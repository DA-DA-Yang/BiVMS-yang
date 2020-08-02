#ifndef SEEDITEM_H
#define SEEDITEM_H

#include <QGraphicsItem>

class SeedItem : public QGraphicsItem
{
public:
	SeedItem();
	~SeedItem();
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	QPainterPath shape() const;
	QPainterPath m_image_path;
private:
};


class BGpointItem : public QGraphicsItem
{
public:
	BGpointItem();
	~BGpointItem();
	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	QPainterPath shape() const;
private:
	QPainterPath m_shape;
};

#endif // STARTPOINT_H
