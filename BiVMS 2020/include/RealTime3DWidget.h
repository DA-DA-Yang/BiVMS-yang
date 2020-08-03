/***************************************************************************
**  BiVMS 2020                                                            **
**  In MainWnd                                                            **
**  A widget for 3D realtime measurement                                  **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QWidget>
#include "ui_RealTime3DWidget.h"
#include "DEFINE.h"
#include "ShowWidget.h"
#include "RealTime3DToolBar.h"
#include "DrawingDock.h"
#include "FlagPlugin.h"
#include "Communication.h"

class RealTime3DWidget : public QWidget
{
	Q_OBJECT

public:
	RealTime3DWidget(QWidget *parent = Q_NULLPTR);
	~RealTime3DWidget();
	inline void setToolBar(RealTime3DToolBar* toolBar) noexcept
	{
		//设置工具栏
		m_toolBar = toolBar;
		if (!m_toolBar)
			return;
		connect(m_toolBar->UI().actionOpenCamera, &QAction::triggered, this, &RealTime3DWidget::_openCamera);
		connect(m_toolBar->UI().actionCloseCamera, &QAction::triggered, this, &RealTime3DWidget::_closeCamera);
		connect(m_toolBar->UI().actionStartCompute, &QAction::triggered, this, &RealTime3DWidget::_startCompute);
		connect(m_toolBar->UI().actionFinishCompute, &QAction::triggered, this, &RealTime3DWidget::_finishCompute);

	}
	inline void setDrawingDock(DrawingDock* drawingDock) noexcept
	{
		m_drawingDock = drawingDock;
	}
	//设置通信，该界面需要设置三个通信端口
	inline void setCommunication(Communication* cMessage, Communication* cImage, Communication* cData) noexcept
	{
		m_communicationMessage = cMessage;
		m_communicationImage = cImage;
		m_communicationData = cData;
		//注意分清楚通信端口连接对象
		connect(m_communicationMessage, &Communication::signalOpenCamera, this, &RealTime3DWidget::_openCamera);
		connect(m_communicationMessage, &Communication::signalCloseCamera, this, &RealTime3DWidget::_closeCamera);
	}
private:
	void _openCamera() noexcept;
	void _closeCamera() noexcept; 
	void _updateShow() noexcept;
	void _updateServantShow() noexcept;
	void _startCompute() noexcept;
	void _finishCompute() noexcept;
	void _sendServantImg() noexcept;
	void _sendServantData() noexcept;

private slots:
	void _slotAddPOI() noexcept;
	void _slotDeletePOI(int index) noexcept;
	void _slotClearPOI() noexcept;

private:
	Ui::RealTime3DWidget ui;
	Communication*         m_communicationMessage = nullptr;   //命令通信端口
	Communication*         m_communicationImage = nullptr;   //图像通信端口
	Communication*         m_communicationData = nullptr;      //数据通信端口
	RealTime3DToolBar*     m_toolBar = nullptr;        //工具栏
	ShowWidget*            m_showMaster = nullptr;     //图像显示界面
	ShowWidget*            m_showServant = nullptr;    //图像显示界面
	DrawingDock*           m_drawingDock = nullptr;    //位移曲线显示界面
	QVector<QRgb>          m_grayColourTable;          //灰度图像颜色表
	QVector<QPoint>        m_POIs;                     //选择的POI
	QTimer*                m_timer=nullptr;            //控制显示的计时器           
	QImage                 m_masterImg;                //主站图像
	QImage                 m_servantImg;               //辅站图像
	bool                   m_computing{ false };       //判断是否在计算中
	QTime                  m_timeMasterShow;           //用来本站画面计算显示帧率的时间
	QTime                  m_timeServantShow;          //用来辅站画面计算显示帧率的时间
	int                    m_FPSMasterShow{};          //本站显示帧率
	int                    m_FPSServantShow{};         //辅站显示帧率
	QTime                  m_computeTime;              //用来统计计算时间

	
};
