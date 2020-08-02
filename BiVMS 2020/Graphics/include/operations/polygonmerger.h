#pragma once

#include "utils/editablegraphicsscene.h"
#include "utils/shape_item.h"

class PolygonMerger 
{
public:
	PolygonMerger(EditableGraphicsScene* scene, AreaItem* area1, AreaItem* area2):
	  _scene(scene), fixedArea(area1), movingArea(area2), _areaMerged(nullptr), _isMerged(false) {}

	void exec();
	void undo();

	void MergeMove() ;
	void merge();


	bool IsMerged() {return _isMerged;}
	AreaItem* GetMergedArea() {return _areaMerged;}

private:

	EditableGraphicsScene* _scene;

	AreaItem* fixedArea;
	AreaItem* movingArea;
	AreaItem* _areaMerged;

	bool _isMerged;

};
