/***************************************************************************
**  BiVMS 2020                                                            **
**  A Dialog to creat Project infomation                                   **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/

#pragma once
#include <QDialog>
#include "ui_NewProjectDlg.h"
#include <QFileDialog>
#include <QFile>     
#include <QtXml\QtXml>  
#include <QtXml\QDomDocument> 
#include "MyMessage.h"
#include "project.h"
#include "Communication.h"

class NewProjectDlg : public QDialog
{
	Q_OBJECT

public:
	NewProjectDlg(QWidget *parent = Q_NULLPTR);
	~NewProjectDlg();

	//拷贝项目类共享指针
	inline void copyProjectPtr(Project* ptr) noexcept { m_project = ptr; }
	//判断项目是否创建成功
	inline bool isCreateProject() noexcept { return m_isCreateProject; }
	inline void setCommunication(Communication* communication) noexcept
	{
		m_communicationMessage = communication;
		connect(m_communicationMessage, &Communication::connected, this, &NewProjectDlg::_slot_get_Connect);
	}

private:
	///响应对话框的按钮-函数
	//建立通信
	void _slot_get_Connect();
	//建立主站通信
	void _slot_pBt_Connect();
	//获取当前时间
	void _slot_get_CurrentTime();
	//保存项目路径
	void _slot_choose_FilePath();
	//确认并生成文件夹等
	void _slot_Ok();
	//取消，退出项目创建
	void _slot_Cancel();
	
private:
	//初始化参数
	void _init_Infomation();
	//得到输入的信息
	void _get_Infomation();
	//将输入的信息传给项目类
	void _send_Infomation();

private:
	QString m_projectName{};										//项目名称
	QString m_surveyor{};											//测量人员
	QString m_location{};											//测量地点
	QString m_time{};												//测量时间
	QString m_filePath{};											//文件路径
	bool m_isConnect = false;									//判断设备是否链接
	bool m_isCreateProject = false;								//判断项目是否创建
	History *m_history = nullptr;								//创建历史成员变量
	//std::shared_ptr<Project> m_project = nullptr;				//项目信息成员变量
	Project *m_project = nullptr;								//项目信息成员变量
	Communication* m_communicationMessage = nullptr;                   //通信
private:
	Ui::NewProjectDlg ui;

};
