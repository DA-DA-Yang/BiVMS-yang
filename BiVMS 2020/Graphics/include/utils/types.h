#pragma once

enum class GraphicEditorStatus
{
	UNUSED = -1,
	READY = 0,
	DRAW_RECT = 1,
	DRAW_CIRC = 2,
	DRAW_POLY = 3,
	DRAW_POINT = 4,
	CLIP_RECT = 5,
	CLIP_CIRC = 6,
	CLIP_POLY = 7,
	AUTO_SEED = 8,
	DEL_SHAPE = 9,
	DEL_NODE = 10,
	SELECT = 11,
	DRAW_BGPOINT=12,
	DRAW_CALIBLINE=13
};