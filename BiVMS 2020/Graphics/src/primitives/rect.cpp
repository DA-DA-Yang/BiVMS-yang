#include "primitives/rect.h"
#include "utils/shape_item.h"

/* initialize drawing rectangular */
void RectPainter::exec()
{
	/* each area node invisible */
	foreach (AreaItem* area, _scene->m_shapes)
		area->SetNodesVisible(false);

	/* initialize the area */
	_rect_area = new AreaItem();
	_rect_area->setPolygon(QPolygonF(QRectF(QPointF(0,0),QPointF(0,0))));
	_scene->addItem(_rect_area);
	_rect_area->setPos(topLeft);
	_scene->m_shapes << _rect_area;

	/* initialize the nodes of the area */
	for (int i = 0; i < 4; i++) {
		NodeItem* node = new NodeItem(_rect_area);
		_rect_area->node_list << node;
		_scene->addItem(node);
		node->setPos(topLeft);
	}	

	/* set the area and nodes visible */
	_rect_area->setVisible(true);
	_rect_area->SetNodesVisible(true);
	_scene->CreateCursorLabel();     // show the coordinate when mouse pressed

}
/* when drawing the rectangle, the rectangle move as the second point move */
void RectPainter::move()
{
	if (!_scene->m_shapes.isEmpty()) {
		QRectF rect(topLeft,rightButtom);

		/* update the rectangle area */
		_rect_area = _scene->m_shapes.last();
		_rect_area->setPolygon(QPolygonF(rect.translated(-topLeft)));

		/* update the node position */
		_rect_area->node_list.value(0)->setPos(rect.topLeft());
		_rect_area->node_list.value(1)->setPos(rect.topRight());
		_rect_area->node_list.value(2)->setPos(rect.bottomRight());
		_rect_area->node_list.value(3)->setPos(rect.bottomLeft());

		if (_scene->m_is_merge_poly){
			_scene->m_selectedregions = _scene->m_shapes.length() - 1;
			_scene->AddInitSet();
			_scene->AddRegionMove(_rect_area);
		}
		_scene->MoveCursorLabel(rightButtom);  // move the coordinate label
	}
}
/* the mouse pressed, end drawing rectangular */
void RectPainter::merge()
{
	_rect_area = _scene->m_shapes.last();
	_rect_area->SetNodesVisible(true);

	if (_scene->m_is_merge_poly){
		_scene->AddInitSet();
		_scene->AddRegionPress(_rect_area);
	}
	_scene->DeleteCursorLabel();
}

void RectPainter::undo()
{
	_scene->removeItem(_rect_area);
	for(int i = 0; i<_rect_area->node_list.length() ;i++)
		_scene->removeItem(_rect_area->node_list.value(i));
	_scene->m_shapes.removeOne(_rect_area);
}

void RectPainter::redo()
{
	_scene->addItem(_rect_area);
	for(int i = 0; i<_rect_area->node_list.length() ;i++)
		_scene->addItem(_rect_area->node_list.value(i));
	_scene->m_shapes<<_rect_area;
}
