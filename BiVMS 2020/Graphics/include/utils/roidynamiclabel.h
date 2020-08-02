#ifndef AOICURSORPOSITIONLABEL_H
#define AOICURSORPOSITIONLABEL_H

#include <QGraphicsTextItem>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QPointF>
#include <QRectF>
#include <QPainter>

class AOICursorPositionLabel : public QGraphicsTextItem
{
public:
	AOICursorPositionLabel(const QString &text, QGraphicsItem *parent = 0);
	~AOICursorPositionLabel();

private:
	void paint(QPainter* painter, const QStyleOptionGraphicsItem *option, QWidget* widget);
};
#endif // AOICURSORPOSITIONLABEL_H
