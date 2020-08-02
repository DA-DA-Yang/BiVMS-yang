#include "utils/handcursorevents.h"

void HandCursorEvent::press()
{
	/* select area */
	if(!_scene->m_shapes.isEmpty()) {

		AreaItem* selectedArea = nullptr;  // select the selected area
		foreach (AreaItem* area, _scene->m_shapes)
			if (area->PointInArea(area->mapFromScene(handPos)))
				selectedArea = area;

		if (selectedArea != nullptr) { // set the node visible
			foreach (AreaItem* area, _scene->m_shapes) {
				area->setVisible(true);
				area->SetNodesVisible(false);
			}
			selectedArea->SetNodesVisible(true);
		}
	}

	/* show the coordinate when mouse press on the seed or node */
	if ( (_scene->m_translate_seed = _scene->cursor_captured_seed(handPos)) != nullptr ) {  // if press on the seed
		_scene->m_is_seed_selected = true;
		_scene->m_last_seed_pos = QPointF(_scene->m_translate_seed->scenePos());
		_scene->CreateCursorLabel();
	} else if (_scene->cursor_captured_node(handPos) != nullptr) { // if press on the node
		_scene->CreateCursorLabel();
	}

}

void HandCursorEvent::move()
/* when hand on the node, the node become red */
{
	foreach (AreaItem* area, _scene->m_shapes)
		area->SetNodesSelect(handPos);
}
