#pragma once

#include "utils/editablegraphicsscene.h"
#include "utils/shape_item.h"
#include "nodeitem.h"
#include <QGraphicsScene>
#include <QGraphicsView>

class HandCursorEvent
{
public:
	HandCursorEvent(EditableGraphicsScene* scene, QPointF point1)
		: _scene(scene),handPos(point1){}

	void press(); //select a target
	void move();
private:

	EditableGraphicsScene* _scene;
	QPointF handPos;
};
