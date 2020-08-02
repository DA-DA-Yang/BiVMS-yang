#pragma once
#include <QPointF>
#include "utils/editablegraphicsscene.h"

class MouseTrackCommand
{
public:
	MouseTrackCommand(EditableGraphicsScene* scene, QPointF point1) : _scene(scene), point(point1) {}
	void exec();

private:
	QPointF point;
	EditableGraphicsScene* _scene;
};
