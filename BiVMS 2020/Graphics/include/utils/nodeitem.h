#ifndef AOINODE_H
#define AOINODE_H

#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class AreaItem;

// class EllipseArea;

using QGrSME = QGraphicsSceneMouseEvent;

class NodeItem : public QGraphicsItem
{
public:
	NodeItem(AreaItem* parent);
	~NodeItem();

	QRectF boundingRect() const;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget *widget);
 	void setSel(bool _select);

	void set_parent(const AreaItem* _region);

	bool dragOver;
	QString marker;
	AreaItem* parent_;

protected:
	void mousePressEvent(QGrSME *e);
	void mouseMoveEvent(QGrSME *e);
	void mouseReleaseEvent(QGrSME *e);

private:
	QColor color;

};

#endif // AOINODE_H
