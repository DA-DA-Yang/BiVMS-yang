/***************************************************************************
**  BiVMS 2020                                                            **
**  A plugin for StartPage                                                **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once
#include <QWidget>
#include "ui_RecentPlugin.h"

class RecentPlugin : public QWidget
{
	Q_OBJECT

public:
	RecentPlugin(QWidget *parent = Q_NULLPTR);
	~RecentPlugin();

public:
	//显示文件名
	inline void set_projectName(QString projectname) {
		ui.lineEdit_projectName->setText(projectname);
		ui.lineEdit_projectName->setStyleSheet("background:transparent;border-width:0;border-style:outset");
	}
	//显示路径
	inline void set_projectPath(QString projectpath) {
		ui.lineEdit_projectPath->setText(projectpath);
		ui.lineEdit_projectPath->setStyleSheet("background:transparent;border-width:0;border-style:outset");
		
	}
	//设置ID号
	inline void set_ID(int index) {
		m_index = index;
	}

signals:
	void signalclose(int index);

public:
	void enterEvent(QEvent * event);
	void leaveEvent(QEvent * event);
	void send_close();

private:
	int m_index = 0;

private:
	Ui::RecentPlugin ui;
};
