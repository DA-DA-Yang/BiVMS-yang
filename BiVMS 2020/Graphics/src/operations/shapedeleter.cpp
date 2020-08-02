#include "operations/shapedeleter.h"
#include "utils/shape_item.h"

void DeleteAreaCommand::exec()
/* delete the area if point in area */
{
	foreach(AreaItem* area, _scene->m_shapes) 
	{
		if(area->PointInArea(area->mapFromScene(_p1)))
		{
			_scene->DelArea(area);
			_scene->m_shapes.removeOne(area);
		}
	}
}

void DeleteAreaCommand::undo()
{

}