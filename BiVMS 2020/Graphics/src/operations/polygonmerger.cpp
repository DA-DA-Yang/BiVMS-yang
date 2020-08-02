#include "operations/polygonmerger.h"
#include "utils/nodeitem.h"
#include <QList>
#include <QPointF>

void PolygonMerger::exec()
{
}

void PolygonMerger::MergeMove()
{
	//条件：如果移动区域的点小于3个则不做操作
	if(movingArea->node_list.length() <= 2)
		return;

	if (_scene->AreaContainArea(movingArea,fixedArea)) {   // if movingArea contain fixedArea
		fixedArea->setVisible(false);
		fixedArea->SetNodesVisible(false);
		return;
	}

	QVector<AreaItem* > fixedAreas = _scene->AreaToAreas(fixedArea);
	QVector<AreaItem* > movingAreas = _scene->AreaToAreas(movingArea);

	AreaItem* fixedAreaMain = fixedAreas.first();  // the outer polygon of the fixed area
	AreaItem* movingAreaMain = movingAreas.first(); // the outer polygon of the moving area

	AreaItem* movingAreaMainCopy = _scene->CopyArea(movingAreaMain);
	QList<QPointF> resultAreaPointsList = _scene->AddAnyRegion(fixedAreaMain, movingAreaMainCopy, false);

	if(_scene->AreaContainArea(fixedArea,movingArea)) {
		fixedArea->setVisible(true);
		fixedArea->SetNodesVisible(false);
		return;
	} else if (resultAreaPointsList.isEmpty()) {
		if (!_scene->AreaIntersectArea(fixedAreaMain,movingArea)) {  // if moving area and fixed area don't intersect
			fixedArea->setVisible(true);
			fixedArea->SetNodesVisible(false);
			return;
		}

		/* here there is problem, resultAreaPointsList maybe not the outer area */
		resultAreaPointsList = _scene->AreaToPointList(fixedAreaMain) << QPointF(-1.0,-1.0);

		for (int i = 1; i < fixedAreas.count(); i++)
		{
			if (_scene->AreaContainArea(fixedAreas[i],movingAreaMain))
				return;

			bool contain = true;  // if mainArea contain all inner area, contain is true 
			QPolygonF  movingAreaMainPolygon = movingAreaMain->polygon();

			for (QList<NodeItem* >::iterator nodeIterator = fixedAreas[i]->node_list.begin(); nodeIterator != fixedAreas[i]->node_list.end(); nodeIterator++)
			{
				QPolygonF mainPolygon = movingAreaMainPolygon.translated(movingAreaMain->pos());
				if (!mainPolygon.containsPoint((*nodeIterator)->scenePos(),Qt::OddEvenFill))
					contain = false;
			}

			if (!contain)
			{  // we need to clip
				AreaItem* movingAreaMainCopy = _scene->CopyArea(movingAreaMain);
				QList<QPointF> clipRegionPointList = _scene->ClipAnyRegion(fixedAreas[i], movingAreaMainCopy, false);
				if (!clipRegionPointList.isEmpty()) {
					resultAreaPointsList.append(clipRegionPointList);
				} 
				else  // we do not need to clip
				{
					QList<QPointF> tempList = _scene->AreaToPointList(fixedAreas[i]); // add the inner polygon to result list
					tempList << QPointF(-1.0,-1.0);
					resultAreaPointsList.append(tempList);
				}
			} 
		}

		for (int i = 1; i < movingAreas.count();i++)
		{
			QList<QPointF> tempList = _scene->AreaToPointList(movingAreas[i]) ;
			tempList << QPointF(-1.0,-1.0);
			resultAreaPointsList.append(tempList);
		}	

		_areaMerged = _scene->PListToArea(resultAreaPointsList);
		_scene->addItem(_areaMerged);

		fixedArea->setVisible(false);
		fixedArea->SetNodesVisible(false);

		movingArea->pen_style = true;
		movingArea->transparency_count=0;

		if (_scene->m_selshapekg)
			_scene->m_selshapekg = false;
		else
			movingArea->SetNodesVisible(false);

		_isMerged = true;

		return;
	}

	for (int i = 1; i < fixedAreas.count(); i++)
	{
		bool contain = true;  // if mainArea contain all inner area, contain is true 
		QPolygonF  movingAreaMainPolygon = movingAreaMain->polygon();

		for (QList<NodeItem* >::iterator nodeIterator = fixedAreas[i]->node_list.begin(); nodeIterator != fixedAreas[i]->node_list.end(); nodeIterator++)
		{
			QPolygonF mainPolygon = movingAreaMainPolygon.translated(movingAreaMain->pos());
			if (!mainPolygon.containsPoint((*nodeIterator)->scenePos(),Qt::OddEvenFill))
				contain = false;
		}

		if (!contain)
		{  // we need to clip
			AreaItem* movingAreaMainCopy = _scene->CopyArea(movingAreaMain);
			QList<QPointF> clipRegionPointList = _scene->ClipAnyRegion(fixedAreas[i], movingAreaMainCopy, false);
			if (!clipRegionPointList.isEmpty())
				resultAreaPointsList.append(clipRegionPointList);
			else  // we do not need to clip
			{
				QList<QPointF> tempList = _scene->AreaToPointList(fixedAreas[i]); // add the inner polygon to result list
				tempList << QPointF(-1.0,-1.0);
				resultAreaPointsList.append(tempList);
			}
		}
	}

	for (int i = 1; i < movingAreas.count();i++)
	{
		QList<QPointF> tempList = _scene->AreaToPointList(movingAreas[i]) ;
		tempList << QPointF(-1.0,-1.0);
		resultAreaPointsList.append(tempList);
	}	

	_areaMerged = _scene->PListToArea(resultAreaPointsList);
	_scene->addItem(_areaMerged);

	fixedArea->setVisible(false);
	fixedArea->SetNodesVisible(false);

	movingArea->pen_style = true;
	movingArea->transparency_count=0;

	if (_scene->m_selshapekg)
		_scene->m_selshapekg = false;
	else
		movingArea->SetNodesVisible(false);

	_isMerged = true;
}

void PolygonMerger::merge()
{
	if(movingArea->node_list.length() <= 2)
		return;

	QVector<AreaItem*> fixedAreas = _scene->AreaToAreas(fixedArea);
	QVector<AreaItem*> movingAreas = _scene->AreaToMulitArea(movingArea);

	AreaItem* fixedAreaMain = fixedAreas.first();
	AreaItem* movingAreaMain = movingAreas.first();

	AreaItem* movingAreaMainCopy = _scene->CopyArea(movingAreaMain);
	QList<QPointF> resultAreaPointsList = _scene->AddAnyRegion(fixedAreaMain, movingAreaMainCopy, false);

	if(_scene->AreaContainArea(fixedArea,movingArea)) {
		return;
	} else if (_scene->AreaContainArea(movingArea,fixedArea)) {
		_scene->DelArea(fixedArea);   // delete the movingArea
		_scene->m_shapes.removeOne(fixedArea);
		_areaMerged = movingArea;
		movingArea->item_select = true;
		_isMerged = true;
		return;
	} else if (resultAreaPointsList.isEmpty()) {
		if (!_scene->AreaIntersectArea(fixedAreaMain,movingArea))
			return;

		resultAreaPointsList = _scene->AreaToPointList(fixedAreaMain) << QPointF(-1.0,-1.0);
		for (int i = 1; i < fixedAreas.count(); i++)
		{
			if (_scene->AreaContainArea(fixedAreas[i],movingAreaMain))
				return;

			bool contain = true;  // if mainArea contain all inner area, contain is true 
			QPolygonF  movingAreaMainPolygon = movingAreaMain->polygon();

			for (QList<NodeItem* >::iterator nodeIterator = fixedAreas[i]->node_list.begin(); nodeIterator != fixedAreas[i]->node_list.end(); nodeIterator++)
			{
				QPolygonF mainPolygon = movingAreaMainPolygon.translated(movingAreaMain->pos());
				if (!mainPolygon.containsPoint((*nodeIterator)->scenePos(),Qt::OddEvenFill))
					contain = false;
			}

			if (!contain)
			{  // we need to clip
				AreaItem* movingAreaMainCopy = _scene->CopyArea(movingAreaMain);
				QList<QPointF> clipRegionPointList = _scene->ClipAnyRegion(fixedAreas[i], movingAreaMainCopy, false);
				if (!clipRegionPointList.isEmpty()) {
					resultAreaPointsList.append(clipRegionPointList);
				} 
				else  // we do not need to clip
				{
					QList<QPointF> tempList = _scene->AreaToPointList(fixedAreas[i]); // add the inner polygon to result list
					tempList << QPointF(-1.0,-1.0);
					resultAreaPointsList.append(tempList);
				}
			} 
		}

		for (int i = 1; i < movingAreas.count();i++)
		{
			QList<QPointF> tempList = _scene->AreaToPointList(movingAreas[i]) ;
			tempList << QPointF(-1.0,-1.0);
			resultAreaPointsList.append(tempList);
		}	

		_areaMerged = _scene->PListToArea(resultAreaPointsList);
		_scene->addItem(_areaMerged);

		_scene->DelArea(fixedArea);   // delete the fixedArea
		_scene->DelArea(movingArea);   // delete the movingArea
		_scene->m_shapes.removeOne(fixedArea);
		_scene->m_shapes.removeOne(movingArea);

		_scene->m_shapes << _areaMerged;
		_areaMerged->item_select = true;

		_isMerged = true;

		return;
	}

	for (int i = 1; i < fixedAreas.count(); i++)
	{

		bool contain = true;  // if mainArea contain all inner area, contain is true 
		QPolygonF  movingAreaMainPolygon = movingAreaMain->polygon();

		for (QList<NodeItem* >::iterator nodeIterator = fixedAreas[i]->node_list.begin(); nodeIterator != fixedAreas[i]->node_list.end(); nodeIterator++)
		{
			QPolygonF mainPolygon = movingAreaMainPolygon.translated(movingAreaMain->pos());
			if (!mainPolygon.containsPoint((*nodeIterator)->scenePos(),Qt::OddEvenFill))
				contain = false;
		}

		if (!contain)
		{  // we need to clip
			AreaItem* movingAreaMainCopy = _scene->CopyArea(movingAreaMain);
			QList<QPointF> clipRegionPointList = _scene->ClipAnyRegion(fixedAreas[i], movingAreaMainCopy, false);
			if (!clipRegionPointList.isEmpty())
				resultAreaPointsList.append(clipRegionPointList);
			else  // we do not need to clip
			{
				QList<QPointF> tempList = _scene->AreaToPointList(fixedAreas[i]); // add the inner polygon to result list
				tempList << QPointF(-1.0,-1.0);
				resultAreaPointsList.append(tempList);
			}
		}
	}

	for (int i = 1; i < movingAreas.count();i++)
	{
		QList<QPointF> tempList = _scene->AreaToPointList(movingAreas[i]) ;
		tempList << QPointF(-1.0,-1.0);
		resultAreaPointsList.append(tempList);
	}	

	_areaMerged = _scene->PListToArea(resultAreaPointsList);
	_scene->addItem(_areaMerged);

	_scene->DelArea(fixedArea);   // delete the fixedArea
	_scene->DelArea(movingArea);   // delete the movingArea
	_scene->m_shapes.removeOne(fixedArea);
	_scene->m_shapes.removeOne(movingArea);

	_scene->m_shapes << _areaMerged;
	_areaMerged->item_select = true;

	_isMerged = true;
}

void PolygonMerger::undo()
{
	_scene->DelArea(_areaMerged);
	_scene->m_shapes.removeOne(_areaMerged);

	_scene->addItem(fixedArea);
	for(int i = 0; i<fixedArea->node_list.length() ;i++)
	{
		_scene->addItem(fixedArea->node_list.value(i));
	}
	_scene->m_shapes<<fixedArea;
}