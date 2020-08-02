#include "utils/roidynamiclabel.h"

AOICursorPositionLabel::AOICursorPositionLabel(const QString &text, QGraphicsItem *parent)
{
}

AOICursorPositionLabel::~AOICursorPositionLabel()
{
}

void AOICursorPositionLabel::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	painter->setBrush(QColor(131,225,131));
	painter->drawRect(boundingRect());
	painter->setRenderHint(QPainter::Antialiasing,true);

	QGraphicsTextItem::paint(painter,option,widget);
}
