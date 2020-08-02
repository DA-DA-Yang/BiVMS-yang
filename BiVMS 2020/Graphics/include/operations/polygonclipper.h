#pragma once
#include "shapeclippingevents.h"
#include "utils/editablegraphicsscene.h"

class PolygonClipper : public ShapeClippingEvents
{
public:
	PolygonClipper(EditableGraphicsScene* scene, QPointF point1) 
		: _scene(scene), _p1(point1){}

	void exec();
	void undo();
	void move();
	void clip();
	void next();

private:
	QPointF _p1;
	AreaItem* _polygon_area;

	EditableGraphicsScene* _scene;
};
