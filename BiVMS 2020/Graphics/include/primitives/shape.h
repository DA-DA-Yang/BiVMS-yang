#pragma once
#include "utils/shapeevents.h"
#include "utils/editablegraphicsscene.h"
#include "utils/shape_item.h"

class ShapePainter : public ShapeEvent
{
public:
	ShapePainter(EditableGraphicsScene* scene, QList<QPointF> pList)
		: _scene(scene),_pList(pList) {}

	void exec();
	void undo();


private:
	AreaItem* _rect_area;
	QList<QPointF> _pList;

	EditableGraphicsScene* _scene;
};
