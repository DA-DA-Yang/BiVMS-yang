#pragma once

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QGraphicsTextItem>
#include <QGraphicsEllipseItem>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QPointF>
#include <QStack>
#include <cmath>
#include "3rdparty/FastDelegate.h"
#include "utils/shape_item.h"
#include "nodeitem.h"
#include "seeditem.h"
#include "wheelscalablegraphicsscene.h"
#include "roidynamiclabel.h"
#include "types.h"
#include "property.hpp"

namespace fdlg = fastdelegate;

using QGrSME = QGraphicsSceneMouseEvent;
template<class _Ty>
using QList2D = QList<QList<_Ty>>;
using Rgn2D = QList<QPointF>;

typedef struct _EQUATIONPARA 
{ 
	double   a; //x的参数 
	double   b; //y的参数(为1或0) 
	double   c; //常数 
} EQUATIONPARA; 

typedef struct RegionLinePt
{
	int flag;
	double x;
	double y;
	QString marker;
	
	struct RegionLinePt *next; 

} ClipRegion;

class EditableGraphicsScene : public WheelScalableGraphicsScene
{
	Q_OBJECT
		//Q_PROPERTY(int transparency READ transparency WRITE transparency)
	using _MyStatus = GraphicEditorStatus;
public:
	using status_type = _MyStatus;

	EditableGraphicsScene(QObject* parent = nullptr);
	~EditableGraphicsScene();

public:
	QList<QPointF> ClipAnyRegion(ShapeItem* areaMain, ShapeItem* areaClip,bool clipType);//clipType==true 裁减区域按顺时针排列
	QList<QPointF> AddAnyRegion(ShapeItem* areaMain, ShapeItem* areaClip,bool clipType);//clipType==true 添加区域按顺时针排列
	QList<QPointF> NodeCrossRegion(ShapeItem* area,NodeItem* nodegrabbed);//拖动node,与选区其他边线产生交点
	double CalArea(ShapeItem* area);
	void ConvertAreaNodeSort(ShapeItem* area);
	bool IsIntersect(QPointF P11, QPointF P12, QPointF P21, QPointF P22, QPointF P);//两条线段是否相交
	bool IsInRegion(double x, double y, ShapeItem* area);
	bool IsInLine(double x, double y, double x1,double y1,double x2,double y2);
	bool CalLineJoint(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double *xx, double *yy);
	int  ClipResultNum(QList<QPointF> qList);
	void AddRegionPress(ShapeItem* _area);
	void AddRegionMove(ShapeItem* _area);
	void ClipRegionPress(ShapeItem* _area);
	void ClipRegionMove(ShapeItem* _area);
	void DelArea(ShapeItem* _area);
	void AddInitSet();
	void ClipInitSet();
	void RefreshAreas();
	bool IsContainArea(ShapeItem* _area1, ShapeItem* _area2);//判断area1是否包含area2
	bool AreaContainArea(ShapeItem* set, ShapeItem* subset);  // su add
	bool AreaIntersectArea(ShapeItem* set1, ShapeItem* set2);
	void DrawEllipse(QPointF pt1, QPointF pt2, QPointF pt3);
	void DrawEmptyEllipse(QPointF pt1, QPointF pt2, QPointF pt3);
	void GetVertDeuceLine(QPointF pt1, QPointF   pt2, EQUATIONPARA   &para);
	bool GetIntersectionPoint(EQUATIONPARA para1, EQUATIONPARA para2, QPointF &center_point);
	double GetDistance(long x1, long y1, long x2, long y2);
	QPointF GetStartPoint();
	QPolygonF GetAreaToPolygon(ShapeItem* area);//获取区域的多边形
	ShapeItem* CopyArea(ShapeItem* _tempArea);
	/*return a selection copy from _other region*/
	ShapeItem* cpyFrom(const ShapeItem* _other);
	ShapeItem* PListToArea(QList<QPointF> pList);
	ShapeItem* PListToNoAddArea(QList<QPointF> pList);//由点绘制1个区域
	QVector<ShapeItem*> PListToMulitArea(QList<QPointF> pList);//点的序列到多个区域
	QVector<ShapeItem*> AreaToMulitArea(ShapeItem* area);//单个区域到数组区域
	QList<QPolygonF*> PListToPolygons(QList<QPointF> pointList);
	QList<QPointF> PolygonsToPList(QList<QPolygonF* > polygons, bool isCross);
	void PListToNormalize(QList<QPointF >& pointList);
	void PolygonToCorrectOrder(QList<QPolygonF* >& polygons);
	void AreaLastSel();
	void DrawExternalArea(QList<QPointF> pList);
	void DrawExternalStartPoint(QPointF p);

	void ClearALLStartPoints();
	void ClearALLBGPoints();

	bool DeleteSeed(QPointF point);
	template<typename _Item>
	bool delete_point_item(QPointF point);

	bool DeleteNode(QPointF point);

	QVector<ShapeItem*> AreaDeleteNodeToAreas(ShapeItem* area, NodeItem* node);
	QVector<ShapeItem*> AreaToAreas(ShapeItem* area);
	QPointF CorrectPointInSceneRect(QPointF point);
	QList2D<QPointF> CollectAreas();
	QList<QPointF> CollectStartPoints();
	QList<QPointF> AreaToPointList(ShapeItem* area);
	void DrawExternalStartPoints(QList<QPointF> _pList);
	void ClearLastArea();
	void InOut(float rate);
	void DrawExternalAreas(QList<QList<QPointF>> _pList);
	int posAreaType(QPointF pos);
	int rightButtonAreaType(QPointF* pos);
	void CreateCursorLabel();
	void DeleteCursorLabel();
	void MoveCursorLabel(QPointF pos);
	void ReentrantPolygonClipping(QPolygonF& oldPoly, QRectF rect, QPolygonF& newPoly);

	//<brief> checks if there are any items in the scene </brief>
	bool empty() const noexcept;

	QRectF get_fit_view_rect();
	ShapeItem* cursor_captured_shape(QPointF pos);
	SeedItem* cursor_captured_seed(QPointF pos);
	NodeItem* cursor_captured_node(QPointF pos);
	QSet<SeedItem*> seedInArea(ShapeItem* area);

	/**
	 *\param [idx] point index, [x, y] current coords of the idx-th target
	 */
	void update_point(size_t idx, float_t x, float_t y) noexcept;

	//<brief> set drawing status </brief>
	inline _MyStatus draw(_MyStatus status) noexcept {
		return this->draw_status() = status;
	}
	inline const _MyStatus& draw_status() const noexcept {
		return m_drawstatus;
	}
	inline _MyStatus& draw_status() noexcept {
		return m_drawstatus;
	}
	inline decltype(auto) bg_points() const noexcept {
		return m_bgpoints;
	}
	/*properties*/
public:
	/*inline
	const int& transparency() const
	{return m_transparency;};
	inline
	void SetTransparency(const int& rtransparency)
	{ m_transparency = rtransparency; };*/

	PROPERTY(int, Transparency);
	__set__(Transparency) { m_transparency = Transparency; }
	__get__(Transparency) const { return m_transparency; }

	READONLY_PROPERTY(QList<Rgn2D>, regions);
	__get__(regions) { return _Gather_selections(); }

	READONLY_PROPERTY(QList<QPointF>, points);
	__get__(points) { return _Gather_points(); }

	QList<Rgn2D> _Gather_selections();
	QList<QPointF> _Gather_points();

private:
	void mousePressEvent(QGrSME *e);
	void mouseMoveEvent(QGrSME *e);
	void mouseReleaseEvent(QGrSME * e);
	void mouseDoubleClickEvent(QGrSME* e);
	void keyPressEvent(QKeyEvent *event);
	void setAreaToNode(QPointF point);
	void GetLeftX(QPointF p, double& lp);
	void GetLeftY(QPointF p, double& lp);
	void GetRightX(QPointF p, double& lp);
	void GetRightY(QPointF p, double& lp);

signals:
	void complete();
	void selectedIndex(int index);             //被选择测点的ID
	void mouse_tracking();
	void right_button_drawing();
	void change_cursor_shape(Qt::CursorShape shape);
	void SignalSSSIGPointPicked(QPointF); // the user has picked the SSSIG Point

private slots:
	void SlotPickSSSIGPoint();

public:
	int m_selectedregions;
	int m_grayvalue;
	bool m_selshapekg;
	bool m_is_merge_poly;
	bool m_handcursor;
	bool m_isdrawing;
	bool m_is_cursor_info_visible;
	bool m_is_seed_selected;
	bool m_is_clipping;   /* if is cutting, then m_is_clipping = true */
	QImage m_image;
	SeedItem* m_translate_seed = nullptr;
	QPointF m_last_seed_pos;
	QPointF m_eventpos;
	AOICursorPositionLabel* m_horcursor_info = nullptr;
	AOICursorPositionLabel* m_vercursor_info = nullptr;
	QList<ShapeItem*> m_shapes;
	QList<ShapeItem*> m_shapes_redo;
	QList<SeedItem*> m_seeds;
	QList< QGraphicsSimpleTextItem*> m_numbers;
	QList<SeedItem*> m_endpts;
	QList<BGpointItem*> m_bgpoints;
	ShapeItem* m_temp_shape = nullptr;
	ShapeItem* m_triangle_shape = nullptr;
	QList<NodeItem*> m_inner_nodes;
	QVector<ShapeItem*> m_temp_shapes;
	fdlg::FastDelegate1<QGrSME*> m_mouse_delegate;
	QGraphicsPixmapItem* m_showImageItem = nullptr;
private:
	int m_transparency;
	bool m_is_addpoint;

	QPointF m_last_pressedpos;
	_MyStatus m_drawstatus;
	_MyStatus m_last_drawstatus;  // when pick the SSSIG point, record the draw_status
};

using GraphicsSceneClientEditor = EditableGraphicsScene;
/*Graphics Scene Client Editor for the ROIs or selections*/
using GrSCE = GraphicsSceneClientEditor;