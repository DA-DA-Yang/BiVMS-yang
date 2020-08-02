#include "primitives/shape.h"

void ShapePainter::exec()
{
	_rect_area = new AreaItem();
	_scene->addItem(_rect_area);
	QPointF _Pt(0.0, 0.0);
	QPointF ref_point= _pList.value(0);
	QPolygonF poly = _rect_area->polygon();
	for (int i = 0; i < _pList.length(); i++)
	{
		_Pt = _pList.value(i);
		if (_Pt.x() == 0 && _Pt.y() == 0)
			break;

		NodeItem* node = new NodeItem(_rect_area);
		node->setPos(_Pt);
		_scene->addItem(node);
		_rect_area->node_list << node;
		poly << node->scenePos() - ref_point;
	}

	_rect_area->setPos(ref_point);
	_rect_area->setPolygon(poly);
	_rect_area->setVisible(true);
	_rect_area->SetNodesVisible(true);
	_scene->m_shapes<<_rect_area;

}

void ShapePainter::undo()
{
	_scene->removeItem(_rect_area);
	for(int i = 0; i<_rect_area->node_list.length() ;i++)
	{
		_scene->removeItem(_rect_area->node_list.value(i));
		delete _rect_area->node_list.value(i);

	}
	delete _rect_area;
	_rect_area = nullptr;

	_scene->m_shapes.removeOne(_rect_area);
}
