/***************************************************************************
**  BiVMS 2020                                                            **
**  Mainwnd                                                               **

****************************************************************************
**           Author: CHENG HUANG & DA YANG                                **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QtWidgets/QMainWindow>
#include <QThread>
#include <QTimer>
#include <QTime>
#include "ui_BiVMS2020.h"
#include "StartDlg.h"
#include "Communication.h"
#include "CommunicationDlg.h"
#include "NewProjectDlg.h"
#include "MyMessage.h"
#include "RealTime3DWidget.h"
#include "RealTime3DToolBar.h"
#include "DrawingDock.h"
#include "StartPageWidget.h"
#include "FlagPlugin.h"
#include "ProjectInfoDlg.h"
#include "Project.h"
#include "CommunicationDlg.h"

class BiVMS2020 : public QMainWindow
{
    Q_OBJECT

public:
    BiVMS2020(QWidget *parent = Q_NULLPTR);

protected:
	void closeEvent(QCloseEvent * event);

private:
	void _set_StatusProjectInfo(const QString filePath) noexcept; //����״̬����Ŀ��Ϣ
	void _new_RealTime3DProject() noexcept;    //�½���άʵʱ��Ŀ 
	void _new_Analysis3DProject() noexcept;    //�½���ά������Ŀ
	void _load_Project() noexcept;             //������Ŀ
	void _load_ProjectInformation() noexcept;  //��Ŀ��Ϣ
	void _load_StartPageWidget() noexcept;     //������ʼҳ
	void _load_RealTime3DWidget() noexcept;    //����ʵʱ����
	void _load_Analysis3DWidget() noexcept;    //���ط�������
	void _load_CalibrationWidget() noexcept;   //���ر궨����
	void _load_HardwareControlWidget() noexcept;   //���ر궨����

	void _Exit() noexcept;                     //�˳�����
	void _onCommunicationSetting() noexcept;
	void _onCommunicationConnected() noexcept;
	void _onCommunicationDisconnected() noexcept;
	

private slots:
	void _remove_TabWidget(int index) noexcept; //ȥ��ĳ��ѡ�����
	void _onTabWidgetCurrentIndexChanged() noexcept;

signals:
	void siganlOpenMasterCommunication();
	void siganlOpenServantCommunication();

private:
    Ui::BiVMS2020Class  ui;
	QLabel*             m_statusLabel = nullptr;
	Communication*      m_communicationMessage = nullptr;      //ͨ��
	Communication*      m_communicationData = nullptr;      //ͨ��
	StartDlg*           m_startDlg = nullptr;			//��ʼ����
	StartPageWidget*    m_startPageWidget = nullptr;    //��ʼҳ
	RealTime3DWidget*   m_realTime3DWidget = nullptr;   //��άʵʱ����
	RealTime3DToolBar*  m_realTime3DToolBar = nullptr;  //��άʵʱ����-������
	DrawingDock*        m_drawingDock = nullptr;		//��ͼ����
	NewProjectDlg*      m_newProjectDlg = nullptr;      //�½���Ŀ�Ի���
	ProjectInfoDlg*     m_projectInfoDlg = nullptr;		//��Ŀ��Ϣ�Ի���

	Project *m_project = nullptr;

	//demo
	QWidget* widget = nullptr;
	QWidget* widget2 = nullptr;
	QWidget* widget3 = nullptr;
};
