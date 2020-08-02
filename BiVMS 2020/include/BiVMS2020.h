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
	void _set_StatusProjectInfo(const QString filePath) noexcept; //设置状态栏项目信息
	void _new_RealTime3DProject() noexcept;    //新建三维实时项目 
	void _new_Analysis3DProject() noexcept;    //新建三维分析项目
	void _load_Project() noexcept;             //加载项目
	void _load_ProjectInformation() noexcept;  //项目信息
	void _load_StartPageWidget() noexcept;     //加载起始页
	void _load_RealTime3DWidget() noexcept;    //加载实时界面
	void _load_Analysis3DWidget() noexcept;    //加载分析界面
	void _load_CalibrationWidget() noexcept;   //加载标定界面
	void _load_HardwareControlWidget() noexcept;   //加载标定界面

	void _Exit() noexcept;                     //退出程序
	void _onCommunicationSetting() noexcept;
	void _onCommunicationConnected() noexcept;
	void _onCommunicationDisconnected() noexcept;
	

private slots:
	void _remove_TabWidget(int index) noexcept; //去除某个选项卡界面
	void _onTabWidgetCurrentIndexChanged() noexcept;

signals:
	void siganlOpenMasterCommunication();
	void siganlOpenServantCommunication();

private:
    Ui::BiVMS2020Class  ui;
	QLabel*             m_statusLabel = nullptr;
	Communication*      m_communicationMessage = nullptr;      //通信
	Communication*      m_communicationData = nullptr;      //通信
	StartDlg*           m_startDlg = nullptr;			//开始界面
	StartPageWidget*    m_startPageWidget = nullptr;    //起始页
	RealTime3DWidget*   m_realTime3DWidget = nullptr;   //三维实时界面
	RealTime3DToolBar*  m_realTime3DToolBar = nullptr;  //三维实时界面-工具栏
	DrawingDock*        m_drawingDock = nullptr;		//绘图界面
	NewProjectDlg*      m_newProjectDlg = nullptr;      //新建项目对话框
	ProjectInfoDlg*     m_projectInfoDlg = nullptr;		//项目信息对话框

	Project *m_project = nullptr;

	//demo
	QWidget* widget = nullptr;
	QWidget* widget2 = nullptr;
	QWidget* widget3 = nullptr;
};
