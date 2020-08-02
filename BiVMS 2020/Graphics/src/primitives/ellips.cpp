#include <QPolygonF>
#include "primitives/ellips.h"
#include "utils/shape_item.h"

void EllipsPainter::exec()
/* initialize drawing circle */
{
	/* if the area_triangle exist, delete it */
	if (_scene->m_triangle_shape != NULL) {
		_scene->removeItem(_scene->m_triangle_shape);
		_scene->m_triangle_shape = NULL;
	}

	/* initialize the inner triangle and nodes */
	AreaItem* area_triangle = new AreaItem(); // inner triangle
	NodeItem* node_first = new NodeItem(area_triangle);  // nodes of the triangle
	NodeItem* node_second = new NodeItem(area_triangle);
	area_triangle->setPos(_p1);
	node_first->setPos(_p1);
	node_second->setPos(_p1);

	QPolygonF polygon;
	polygon << QPointF(0,0) << QPointF(0,0);
	area_triangle->setPolygon(polygon);
	area_triangle->node_list << node_first << node_second;
	_scene->m_triangle_shape = area_triangle;
	_scene->addItem(area_triangle);
	_scene->CreateCursorLabel();     // show the coordinate

}

void EllipsPainter::move()
/* drawing the circle, when the mouse move */
{
	AreaItem* area_triangle = _scene->m_triangle_shape;
	NodeItem* node_last = area_triangle->node_list.last();
	node_last->setPos(_p1);

	QPointF pos_refer = area_triangle->node_list.value(0)->scenePos();
	QPolygonF polygon = area_triangle->polygon();
	polygon.last() = node_last->scenePos() - pos_refer;
	area_triangle->pen_style = true;
	area_triangle->setPolygon(polygon);
	if (area_triangle->node_list.count() >= 3)   // drawing the circle
	{
		_scene->DrawEllipse(area_triangle->node_list.value(0)->scenePos(), area_triangle->node_list.value(1)->scenePos(), _p1 ); 
		if (_scene->m_is_merge_poly)  // merge
		{
			_scene->m_selectedregions = _scene->m_shapes.length()-1;
			_scene->AddInitSet();
			_scene->AddRegionMove(_scene->m_shapes.last());
		}
	}

	_scene->MoveCursorLabel(_p1); // move the coordinate label

}

void EllipsPainter::merge()
/* left mouse pressed when drawing circle */
{
	if (_scene->m_triangle_shape->node_list.length() < 3) // drawing the second point of the circle	
	{  
		_ellips_area = new AreaItem(); 
		_scene->addItem(_ellips_area);   // add the circle to the scene
		_scene->m_shapes << _ellips_area;
		_scene->AreaLastSel();

		AreaItem* area_triangle = _scene->m_triangle_shape;
		NodeItem* node_next = new NodeItem(_scene->m_triangle_shape);
		node_next->setPos(_p1);
		_scene->m_triangle_shape->node_list << node_next;
		QPolygonF poly = area_triangle->polygon();
		poly << poly.last();
		_scene->m_triangle_shape->setPolygon(poly);
	} else // drawing the third point of the circle
	{ 
		_scene->removeItem(_scene->m_triangle_shape);
		if (_scene->m_is_merge_poly) {
			_scene->AddInitSet();
			_scene->AddRegionPress(_scene->m_shapes.last());
		}
		_scene->m_shapes.last()->SetNodesVisible(true);
		_scene->m_isdrawing =false;
		_scene->DeleteCursorLabel();  // delete the coordinate label

	}
}

void EllipsPainter::undo()
{
	//_scene->removeItem(_ellips_area);
	//for(int i = 0; i<_ellips_area->node_list.length() ;i++)
	//{
	//	_scene->removeItem(_ellips_area->node_list.value(i));
	//}
	//_scene->m_shapes.removeOne(_ellips_area);
}

void EllipsPainter::redo()
{
	/*_scene->addItem(_ellips_area);
	for(int i = 0; i<_ellips_area->node_list.length() ;i++)
	{
		_scene->addItem(_ellips_area->node_list.value(i));
	}
	_scene->m_shapes<<_ellips_area;*/
}

void EllipsPainter::right_click()
/* when drawing the circle, we press the right button */
{
	if (_scene->m_triangle_shape != NULL)  // if we are drawing the second point or third point
	{
		int num = _scene->m_triangle_shape->node_list.length();
		if (num == 3) // the third point, end drawing
		{
			if (_scene->m_is_merge_poly) // merge
			{
				_scene->AddInitSet();
				_scene->AddRegionPress(_scene->m_shapes.last());
			}
		}

		/* delete the inner triangle and its nodes */
		_scene->removeItem(_scene->m_triangle_shape);
		_scene->m_triangle_shape = NULL;

		_scene->DeleteCursorLabel();  // delete the coordinate label
	}
}
