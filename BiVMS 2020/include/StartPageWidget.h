/***************************************************************************
**  BiVMS 2020                                                            **
**  Start Page															  **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/

#pragma once
#include <QSettings>
#include <QWidget>
#include <Qdir>
#include <Qfile>
#include <QFileDialog>
#include <Qdesktopservices>
#include "History.h"
#include "RecentPlugin.h"
#include "MyPushButton.h"
#include "MyMessage.h"
#include "ui_StartPageWidget.h"
#include "NewProjectDlg.h"

class StartPageWidget : public QWidget
{
	Q_OBJECT
	
public:
	StartPageWidget(QWidget *parent = Q_NULLPTR);
	~StartPageWidget();

signals:
	//创建3D实时项目信号_DescribeOriginal
	void signalnew3DProject();
	//创建后处理项目信号
	void signalnewAnalysis();

private:
	//显示历史创建文件信息
	void _show_History();


private:
	///响应对话框的按钮-函数
	//Label响应3D_RealTime按钮
	void _slot_Describe3DRealTime();
	//Label响应3Analysis按钮
	void _slot_DescribeAnalysis();
	//Label回到最初的状态
	void _slot_DescribeOriginal();
	//发出新建3D实时项目信号
	void _slot_new3DProject();
	//发出新建后处理项目信号
	void _slot_newAnalysis();
	//打开操作说明书
	void _slot_OpenManual();
	//打开联系邮箱
	void _slot_OpenMail();
	//删除历史记录
	void _slot_delete_History(int index);


private:
	History *m_history = nullptr;				//历史信息
	QVector<RecentPlugin*> m_recentPlugin;		//自定义显示控件
	QVector<QString> m_projectname;				//历史文件名
	QVector<QString> m_projectpath;				//历史文件路径
	QVector<QString> m_createtime;				//历史文件创建时间
	QString m_create_filePath;					//创建文件路径
	QString m_create_projectName;				//创建项目名称
	QString m_CurrentTime;						//当前时间
	int m_projectSize;

	

private:
	Ui::StartPageWidget ui;

};




