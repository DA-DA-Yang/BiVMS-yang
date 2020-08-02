#include "primitives/polygon.h"
#include "utils/shape_item.h"

/* initialize drawing polygon */
void PolygonPainter::exec()
{
	/* first make each node invisible */
	foreach (AreaItem* area, _scene->m_shapes)
		area->SetNodesVisible(false);

	/* initialize the area */
	QPolygonF polygon;
	polygon << QPointF(0,0) << QPointF(0,0);
	_polygon_area = new AreaItem();
	_polygon_area->setPolygon(polygon);
	_scene->addItem(_polygon_area);
	_polygon_area->setPos(_p1);
	_scene->m_shapes << _polygon_area;

	/* initialize the nodes of the area */
	NodeItem* node = new NodeItem(_polygon_area);
	NodeItem* node_next = new NodeItem(_polygon_area);
	_polygon_area->node_list << node << node_next;
	_scene->addItem(node);
	node->setPos(_p1);
	_scene->addItem(node_next);
	node_next->setPos(_p1);

	/* make the polygon visible */
	_polygon_area->setVisible(true);
	_polygon_area->SetNodesVisible(true);
	_scene->CreateCursorLabel();

}

void PolygonPainter::next()
/* continue drawing the polygon */
{
	_polygon_area = _scene->m_shapes.last();

	/* add new node */
	NodeItem* node_next = new NodeItem(_polygon_area); 
	node_next->setPos(_p1);
	_polygon_area->node_list << node_next;
	_scene->addItem(node_next);

	/* update the polygon of the areaitem */
	QPolygonF polygon = _polygon_area->polygon();
	polygon << polygon.last();
	_polygon_area->setPolygon(polygon);
}

void PolygonPainter::undo()
{
	_scene->removeItem(_polygon_area);
	for(int i = 0; i<_polygon_area->node_list.length() ;i++)
	{
		_scene->removeItem(_polygon_area->node_list.value(i));
	}
	_scene->m_shapes.removeOne(_polygon_area);
}

void PolygonPainter::redo()
{
	_scene->addItem(_polygon_area);
	for(int i = 0; i<_polygon_area->node_list.length() ;i++)
	{
		_scene->addItem(_polygon_area->node_list.value(i));
	}
	_scene->m_shapes<<_polygon_area;
}

void PolygonPainter::move()
{
	if (_scene->m_shapes.isEmpty()) return;
	_polygon_area = _scene->m_shapes.last();

	QPointF pos_refer = _polygon_area->node_list.value(0)->scenePos();
	NodeItem* node_next = _polygon_area->node_list.last();
	node_next->setPos(_p1);
	_polygon_area->setPos(pos_refer);
	QPolygonF poly = _polygon_area->polygon();
	poly.pop_back();
	poly << node_next->scenePos() - pos_refer;
	_polygon_area->setPolygon(poly);

	if (!_scene->m_is_merge_poly) return;

	_scene->m_selectedregions = _scene->m_shapes.length()-1;
	_scene->AddInitSet();
	_scene->AddRegionMove(_polygon_area);
	_scene->MoveCursorLabel(_p1);
}


void PolygonPainter::merge()
{
	if(_scene->m_shapes.isEmpty()) return;

	_polygon_area = _scene->m_shapes.last();

	if (_polygon_area->node_list.length()<3)
	{
		_scene->DelArea(_polygon_area);
		_scene->m_shapes.removeOne(_polygon_area);
		return;
	}

	_scene->DeleteCursorLabel();

 	if (!_scene->m_is_merge_poly) return;
 	_scene->AddInitSet();
 	_scene->AddRegionPress(_polygon_area);
}