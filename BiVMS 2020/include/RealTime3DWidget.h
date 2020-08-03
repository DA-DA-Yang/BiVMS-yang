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
		//���ù�����
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
	//����ͨ�ţ��ý�����Ҫ��������ͨ�Ŷ˿�
	inline void setCommunication(Communication* cMessage, Communication* cImage, Communication* cData) noexcept
	{
		m_communicationMessage = cMessage;
		m_communicationImage = cImage;
		m_communicationData = cData;
		//ע������ͨ�Ŷ˿����Ӷ���
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
	Communication*         m_communicationMessage = nullptr;   //����ͨ�Ŷ˿�
	Communication*         m_communicationImage = nullptr;   //ͼ��ͨ�Ŷ˿�
	Communication*         m_communicationData = nullptr;      //����ͨ�Ŷ˿�
	RealTime3DToolBar*     m_toolBar = nullptr;        //������
	ShowWidget*            m_showMaster = nullptr;     //ͼ����ʾ����
	ShowWidget*            m_showServant = nullptr;    //ͼ����ʾ����
	DrawingDock*           m_drawingDock = nullptr;    //λ��������ʾ����
	QVector<QRgb>          m_grayColourTable;          //�Ҷ�ͼ����ɫ��
	QVector<QPoint>        m_POIs;                     //ѡ���POI
	QTimer*                m_timer=nullptr;            //������ʾ�ļ�ʱ��           
	QImage                 m_masterImg;                //��վͼ��
	QImage                 m_servantImg;               //��վͼ��
	bool                   m_computing{ false };       //�ж��Ƿ��ڼ�����
	QTime                  m_timeMasterShow;           //������վ���������ʾ֡�ʵ�ʱ��
	QTime                  m_timeServantShow;          //������վ���������ʾ֡�ʵ�ʱ��
	int                    m_FPSMasterShow{};          //��վ��ʾ֡��
	int                    m_FPSServantShow{};         //��վ��ʾ֡��
	QTime                  m_computeTime;              //����ͳ�Ƽ���ʱ��

	
};
