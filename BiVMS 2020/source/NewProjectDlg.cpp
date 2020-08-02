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

//��ʼ������
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
		ui.label_status->setText(QString::fromLocal8Bit("��ǰδ����ͨ�ţ����Ƚ���ͨ��"));
		ui.groupBox_Info->setEnabled(false);
	}
	ui.lineEdit_ProjectName->setText("3D_RealTime_");
	ui.lineEdit_Surveyors->setText("Admin");
	ui.lineEdit_Location->setText("Here");
}

//�õ�����������Ϣ
void NewProjectDlg::_get_Infomation()
{
	
	m_projectName = ui.lineEdit_ProjectName->text();
	m_surveyor = ui.lineEdit_Surveyors->text();
	m_location = ui.lineEdit_Location->text();
	m_time = ui.lineEdit_Time->text();
	m_filePath = ui.lineEdit_FilePath->text();
}

//���������Ŀ��Ϣ���͸���Ŀ��
void NewProjectDlg::_send_Infomation()
{
	m_project->set_projecName(m_projectName);
	m_project->set_surveyor(m_surveyor);
	m_project->set_location(m_location);
	m_project->set_time(m_time);
	m_project->set_filePath(m_filePath);
}

//����ͨ��
void NewProjectDlg::_slot_get_Connect()
{
	QImage image;
	QString filepath = QString::fromLocal8Bit("resource/NewProjectDlg/right.png");
	image.load(filepath);
	int width = image.width();
	ui.label_Icon->setPixmap(QPixmap::fromImage(image));
	ui.label_Icon->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
	ui.label_Icon->setScaledContents(true);
	ui.label_status->setText(QString::fromLocal8Bit("����ͨ�ųɹ�"));
	ui.groupBox_Info->setEnabled(true);
}
//������վͨ��
void NewProjectDlg::_slot_pBt_Connect()
{
	if (m_communicationMessage)
	{
		m_communicationMessage->openMaster();
	}
}

//��ȡ��ǰʱ��
void NewProjectDlg::_slot_get_CurrentTime()
{
	QDateTime current_date_time = QDateTime::currentDateTime();
	m_time = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
	ui.lineEdit_Time->setText(m_time);
}

//������Ŀ·��
void NewProjectDlg::_slot_choose_FilePath()
{
	m_filePath = QFileDialog::getExistingDirectory();
	ui.lineEdit_FilePath->setText(m_filePath);
}

//ȷ�ϲ������ļ��е�
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
		MyMessage* mes = new MyMessage("��ǰδ��������Ŀ���Ƿ��Խ��������棿", false, this);
		if (mes->exec() == QDialog::Accepted)
		{
			this->accept();
		}	
	}
}

//ȡ�����˳���Ŀ����
void NewProjectDlg::_slot_Cancel()
{
	this->reject();
}


