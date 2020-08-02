
#pragma once
#include "shapeclippingevents.h"
#include "utils/editablegraphicsscene.h"
#include <QPointF>

class EllipsClipper : public ShapeClippingEvents
{
public:
	EllipsClipper(EditableGraphicsScene* scene, QPointF point1) : _scene(scene), _p1(point1) {}

	void exec();
	void undo();
	void move();
	void clip();
	void right_click();
private:
	QPointF _p1;
	AreaItem* _ellips_area;
	EditableGraphicsScene* _scene;
};
