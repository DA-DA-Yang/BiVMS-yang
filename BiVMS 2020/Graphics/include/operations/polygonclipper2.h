#pragma once

#include "utils/editablegraphicsscene.h"
#include "utils/shape_item.h"

class PolygonCutCommand 
{
public:
	PolygonCutCommand(EditableGraphicsScene* scene, AreaItem* area1, AreaItem* area2):
	  _scene(scene), fixedArea(area1), movingArea(area2), _areaMerged(nullptr), _isMerged(false) {}

	  void exec();
	  void undo();

	  void ClipMove() ;
	  void ClipDo();


	  bool IsCuted() {return _isMerged;}
	  AreaItem* GetMergedArea() {return _areaMerged;}


	  QVector<AreaItem*>  GetMergedAreaArray() {return areaArray;}

private:

	EditableGraphicsScene* _scene;

	AreaItem* fixedArea;
	AreaItem* movingArea;
	AreaItem* _areaMerged;

	bool _isMerged;

	QVector<AreaItem*> areaArray;

};
