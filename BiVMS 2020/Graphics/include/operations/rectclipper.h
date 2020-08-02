#pragma once
#include <QPointF>
#include "shapeclippingevents.h"
#include "utils/editablegraphicsscene.h"
class RectClipper :
	public ShapeClippingEvents
{
public:
	RectClipper(EditableGraphicsScene* scene, QPointF point1, QPointF point2) : _scene(scene), _p1(point1), _p2(point2) {}
	void exec();
	void undo();
	void move();
	void clip();

private:
	QPointF _p1;
	QPointF _p2;
	AreaItem* _needCutArea;

	EditableGraphicsScene* _scene;
};