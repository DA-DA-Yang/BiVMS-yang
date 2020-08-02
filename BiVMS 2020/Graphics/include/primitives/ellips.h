#pragma once
#include "utils/shapeevents.h"
#include "utils/editablegraphicsscene.h"
#include <QPointF>

class EllipsPainter : public ShapeEvent
{
public:
	EllipsPainter(EditableGraphicsScene* scene, QPointF point1) 
		: _scene(scene), _p1(point1) {}
	void exec();
	void undo();
	void move();
	void merge();
	void right_click();
	void redo();

private:
	QPointF _p1;
	AreaItem* _ellips_area;

	EditableGraphicsScene* _scene;
};
