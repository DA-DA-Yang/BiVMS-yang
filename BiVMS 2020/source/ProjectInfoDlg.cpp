#include "ProjectInfoDlg.h"

ProjectInfoDlg::ProjectInfoDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);	
}
ProjectInfoDlg::ProjectInfoDlg(Project * ptr,QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	m_project = ptr;
	if(m_project)
		_show_projectInfo();
}

ProjectInfoDlg::~ProjectInfoDlg()
{
}

//显示项目信息
void ProjectInfoDlg::_show_projectInfo()
{
	if (m_project->isEmpty())
		return;
	ui.lineEdit_ProjectName->setText(m_project->get_projecName());
	ui.lineEdit_Surveyors->setText(m_project->get_surveyor());
	ui.lineEdit_Location->setText(m_project->get_location());
	ui.lineEdit_Time->setText(m_project->get_time());
	ui.lineEdit_FilePath->setText(m_project->get_filePath());
}