#pragma once
#include <QPointF>

class MouseTracking
{
public:
	MouseTracking(MaskEditorGraphicsScene* scene, QPointF point) 
		: m_scene(scene), m_point(point) {}

	void exec() {};

private:
	QPointF m_point;
	MaskEditorGraphicsScene* m_scene = nullptr;
};
