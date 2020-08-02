#include <operations/ellipsclipper.h>
#include <utils/shape_item.h>

void EllipsClipper::exec()
/* initialize to cut circle */
{
	/* if the inner triangle exist, then delete it */
	if (_scene->m_triangle_shape != NULL)
	{
		_scene->removeItem(_scene->m_triangle_shape);
		_scene->m_triangle_shape = NULL;
	}

	/* initialize the area and nodes */
	AreaItem* area_triangle = new AreaItem();  // inner triangle
	NodeItem* node_first = new NodeItem(area_triangle);
	NodeItem* node_second = new NodeItem(area_triangle);
	area_triangle->setPos(_p1);
	node_first->setPos(_p1);
	node_second->setPos(_p1);
	QPolygonF poly;
	poly << QPointF(0,0) << QPointF(0,0);
	area_triangle->setPolygon(poly);
	area_triangle->node_list << node_first << node_second;
	area_triangle->transparency_count = 0;

	_scene->addItem(area_triangle);	
	_scene->m_triangle_shape = area_triangle;
	_scene->CreateCursorLabel();     // show the coordinate
}

void EllipsClipper::move()
/* when the move mouse, cutting the circle */
{
	AreaItem* area_triangle = _scene->m_triangle_shape;
	NodeItem* node_last = area_triangle->node_list.last();
	node_last->setPos(_p1);  // update the node

	/* update the polygon */
	QPointF pos_refer = area_triangle->node_list.value(0)->scenePos();  // reference point
	QPolygonF polygon = area_triangle->polygon();
	polygon.last() = node_last->scenePos() - pos_refer;
	area_triangle->setPolygon(polygon);
	area_triangle->pen_style = true;

	if (area_triangle->node_list.count() >= 3) // drawing the cutting circle
	{
		_scene->DrawEmptyEllipse(area_triangle->node_list.value(0)->scenePos(), area_triangle->node_list.value(1)->scenePos(), _p1 );
		if (_scene->m_is_merge_poly)
		{
			_scene->ClipInitSet();  // initial clipping
			_scene->ClipRegionMove(_scene->m_shapes.last());
			_scene->m_shapes.last()->pen_style = true;
		}
	}

	_scene->MoveCursorLabel(_p1);   // move the coordinate label
}

void EllipsClipper::clip()
/* continue to cut circle */
{
	if (_scene->m_triangle_shape->node_list.length() < 3)  // the second point of the circle
	{
		AreaItem* area_circle = new AreaItem(); // outer circle
		_scene->addItem(area_circle);
		_scene->m_shapes << area_circle;
		AreaItem* area_triangle = _scene->m_triangle_shape;
		NodeItem* node_next = new NodeItem(_scene->m_triangle_shape);
		node_next->setPos(_p1);
		_scene->m_triangle_shape->node_list << node_next;
		QPolygonF polygon = area_triangle->polygon();
		polygon << polygon.last();
		_scene->m_triangle_shape->setPolygon(polygon);
	}
	else  // end to cut the circle, the third point of the circle
	{
		_scene->removeItem(_scene->m_triangle_shape);
		AreaItem* _area = _scene->m_shapes.last();
		if (_scene->m_is_merge_poly)
		{
			_scene->ClipInitSet();
			_scene->ClipRegionPress(_area);
		}

		_scene->DelArea(_area);
		_scene->m_shapes.removeOne(_area);
		_scene->m_isdrawing =false;
		_scene->DeleteCursorLabel();
	}

}

void EllipsClipper::undo()
{

}

void EllipsClipper::right_click()
/* when cutting the circle, the right mouse is pressed */
{
	if (_scene->m_triangle_shape != NULL)   // if we are drawing the second point or third point
	{
		if (_scene->m_triangle_shape->node_list.length() == 3)  // the third point, end drawing the cutting circle and clip
		{
			AreaItem* _area = _scene->m_shapes.last();  // _area is the circle, the area need to cut
			if (_scene->m_is_merge_poly)  // clip
			{
				_scene->ClipInitSet();
				_scene->ClipRegionPress(_area);
			}
			_scene->DelArea(_area);
			_scene->m_shapes.removeOne(_area);
		}

		/* remove inner triangle */
		_scene->removeItem(_scene->m_triangle_shape);
		_scene->m_triangle_shape = NULL;
		_scene->DeleteCursorLabel(); // delete the coordinate label
	}

}