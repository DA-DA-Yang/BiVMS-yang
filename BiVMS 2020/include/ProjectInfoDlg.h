/***************************************************************************
**  BiVMS 2020                                                            **
**  A Dialog to show Project infomation                                   **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 27.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QDialog>
#include "Project.h"
#include "ui_ProjectInfoDlg.h"

class ProjectInfoDlg : public QDialog
{
	Q_OBJECT

public:
	ProjectInfoDlg(QWidget *parent = Q_NULLPTR);
	ProjectInfoDlg(Project * ptr, QWidget *parent);
	~ProjectInfoDlg();


private:
	void _show_projectInfo();						//显示项目信息
private:			
	//std::shared_ptr<Project> m_project = nullptr; //项目信息成员变量
	Project *m_project = nullptr;

private:
	Ui::ProjectInfoDlg ui;
};
