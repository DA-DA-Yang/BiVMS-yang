#pragma once
#include "utils/shapeevents.h"
#include "utils/editablegraphicsscene.h"

class PolygonPainter :
	public ShapeEvent
{
public:
	PolygonPainter(EditableGraphicsScene* scene, QPointF point1) : _scene(scene), _p1(point1){}

	void exec();
	void undo();
	void move();
	void merge();
	void next();
	void redo();

private:
	QPointF _p1;
	AreaItem* _polygon_area;

	EditableGraphicsScene* _scene;
};
 