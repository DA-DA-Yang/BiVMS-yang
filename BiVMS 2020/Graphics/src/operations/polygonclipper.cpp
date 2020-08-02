#include "operations/polygonclipper.h"
#include "utils/shape_item.h"

void PolygonClipper::exec()
/* initialize to cut polygon */
{
	/* initialize the cutting area */
	_polygon_area = new AreaItem();
	QPolygonF polygon;
	polygon << QPointF(0,0) << QPointF(0,0);
	_polygon_area->setPolygon(polygon);
	_polygon_area->transparency_count = 0;  // set the transparency
	_polygon_area->setPos(_p1);
	_scene->addItem(_polygon_area);
	_scene->m_shapes<<_polygon_area;

	/* initialize the nodes of the area */
	NodeItem* node = new NodeItem(_polygon_area);
	NodeItem* node_next = new NodeItem(_polygon_area);
	node->setPos(_p1);
	node_next->setPos(_p1);
	_polygon_area->node_list << node << node_next;

	_scene->CreateCursorLabel();     // show the coordinate when mouse pressed
}

void PolygonClipper::undo()
{

}
void PolygonClipper::move()
/* when the move mouse, the cutting polygon change */
{
	if (!_scene->m_shapes.isEmpty()) {

		/* update the polygon */
		_polygon_area = _scene->m_shapes.last();
		QPointF pos_refer = _polygon_area->node_list.value(0)->scenePos();  // reference point
		QPolygonF polygon = _polygon_area->polygon();
		NodeItem* node_next = _polygon_area->node_list.last();
		polygon.last() = node_next->scenePos() - pos_refer;
		_polygon_area->setPolygon(polygon);

		/* update the nodes */
		node_next->setPos(_p1);

		if (_scene->m_is_merge_poly) { // clip the polygon
			_scene->ClipInitSet();
			_scene->ClipRegionMove(_polygon_area);
		}

		_scene->MoveCursorLabel(_p1); // move the coordinate label
	}
}


void PolygonClipper::next()
/* continue to cut polygon */
{
	/* update the area */
	_polygon_area = _scene->m_shapes.last();
	NodeItem* node_next = new NodeItem(_polygon_area);
	node_next->setPos(_p1);
	_polygon_area->node_list << node_next;

	/* update the area polygon */
	QPolygonF polygon = _polygon_area->polygon();
	polygon << polygon.last();
	_polygon_area->setPolygon(polygon);
}

void PolygonClipper::clip()
{
	if(_scene->m_shapes.isEmpty())
		return;
	_polygon_area = _scene->m_shapes.last();
	if (_polygon_area->node_list.length()<3)
	{
		_scene->DelArea(_polygon_area);
		_scene->m_shapes.removeOne(_polygon_area);
		return;
	}
	//	_scene->PolygonCancel();//小于三个点取消显示


// 	if (!_scene->m_is_merge_poly)
// 		return;
	_scene->ClipInitSet();
	_scene->ClipRegionPress(_polygon_area);

	_scene->DelArea(_polygon_area);
	_scene->m_shapes.removeOne(_polygon_area);
}