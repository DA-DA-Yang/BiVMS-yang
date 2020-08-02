#include "NewProjectDlg.h"

NewProjectDlg::NewProjectDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	_init_Infomation();
	connect(ui.pbutton_Connect, &QPushButton::clicked, this, &NewProjectDlg::_slot_pBt_Connect);
	connect(ui.pbutton_time, &QPushButton::clicked, this, &NewProjectDlg::_slot_get_CurrentTime);
	connect(ui.pbutton_browse, &QPushButton::clicked, this, &NewProjectDlg::_slot_choose_FilePath);
	connect(ui.pbutton_ok, &QPushButton::clicked, this, &NewProjectDlg::_slot_Ok);
	connect(ui.pbutton_cancel, &QPushButton::clicked, this, &NewProjectDlg::_slot_Cancel);
	
}

NewProjectDlg::~NewProjectDlg()
{
	if (m_history != nullptr) {
		delete m_history;
		m_history = nullptr;
	}
}

//初始化参数
void NewProjectDlg::_init_Infomation()
{
	if (m_history != nullptr) {
		delete m_history;
		m_history = nullptr;
	}
	m_history = new History(this);
	//m_project = std::make_shared<Project>(this);
	
	if (m_isConnect == false) {
		QImage image;
		QString filepath = QString::fromLocal8Bit("resource/NewProjectDlg/wrong.png");
		image.load(filepath);
		ui.label_Icon->setPixmap(QPixmap::fromImage(image));
		ui.label_Icon->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		ui.label_Icon->setScaledContents(true);
		ui.label_status->setText(QString::fromLocal8Bit("当前未建立通信，请先建立通信"));
		ui.groupBox_Info->setEnabled(false);
	}
	ui.lineEdit_ProjectName->setText("3D_RealTime_");
	ui.lineEdit_Surveyors->setText("Admin");
	ui.lineEdit_Location->setText("Here");
}

//得到窗口输入信息
void NewProjectDlg::_get_Infomation()
{
	
	m_projectName = ui.lineEdit_ProjectName->text();
	m_surveyor = ui.lineEdit_Surveyors->text();
	m_location = ui.lineEdit_Location->text();
	m_time = ui.lineEdit_Time->text();
	m_filePath = ui.lineEdit_FilePath->text();
}

//将输入的项目信息发送给项目类
void NewProjectDlg::_send_Infomation()
{
	m_project->set_projecName(m_projectName);
	m_project->set_surveyor(m_surveyor);
	m_project->set_location(m_location);
	m_project->set_time(m_time);
	m_project->set_filePath(m_filePath);
}

//建立通信
void NewProjectDlg::_slot_get_Connect()
{
	QImage image;
	QString filepath = QString::fromLocal8Bit("resource/NewProjectDlg/right.png");
	image.load(filepath);
	int width = image.width();
	ui.label_Icon->setPixmap(QPixmap::fromImage(image));
	ui.label_Icon->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.label_Icon->setScaledContents(true);
	ui.label_status->setText(QString::fromLocal8Bit("建立通信成功"));
	ui.groupBox_Info->setEnabled(true);
}
//建立主站通信
void NewProjectDlg::_slot_pBt_Connect()
{
	if (m_communicationMessage)
	{
		m_communicationMessage->openMaster();
	}
}

//获取当前时间
void NewProjectDlg::_slot_get_CurrentTime()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	m_time = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
	ui.lineEdit_Time->setText(m_time);
}

//保存项目路径
void NewProjectDlg::_slot_choose_FilePath()
{
	m_filePath = QFileDialog::getExistingDirectory();
	ui.lineEdit_FilePath->setText(m_filePath);
}

//确认并生成文件夹等
void NewProjectDlg::_slot_Ok()
{
	_get_Infomation();
	if (!m_filePath.isEmpty()&&!m_projectName.isEmpty())
	{
		_send_Infomation();
		if (m_project->generate_Folder())
		{
			m_project->write_XMl();
			QDateTime current_date_time = QDateTime::currentDateTime();
			QString m_realtime = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
			m_history->write_History(m_projectName,m_filePath, m_realtime);
			m_isCreateProject = true;
			
			this->accept();
		}
	}
	else{
		MyMessage* mes = new MyMessage("当前未创建新项目，是否仍进入主界面？", false, this);
		if (mes->exec() == QDialog::Accepted)
		{
			this->accept();
		}	
	}
}

//取消，退出项目创建
void NewProjectDlg::_slot_Cancel()
{
	this->reject();
}


