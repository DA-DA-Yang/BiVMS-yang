#include "StartPageWidget.h"

StartPageWidget::StartPageWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	_show_History();
	connect(ui.pushButton_3D_RealTime, &MyPushButton::signalHover, this, &StartPageWidget::_slot_Describe3DRealTime);
	connect(ui.pushButton_3D_RealTime, &MyPushButton::signalLeave, this, &StartPageWidget::_slot_DescribeOriginal);
	connect(ui.pushButton_3D_RealTime, &MyPushButton::clicked, this, &StartPageWidget::_slot_new3DProject);
	connect(ui.pushButton_Analysis, &MyPushButton::signalHover, this, &StartPageWidget::_slot_DescribeAnalysis);
	connect(ui.pushButton_Analysis, &MyPushButton::signalLeave, this, &StartPageWidget::_slot_DescribeOriginal);
	connect(ui.pushButton_Analysis, &MyPushButton::clicked, this, &StartPageWidget::_slot_newAnalysis);
	connect(ui.pushButton_Manual, &QPushButton::clicked, this, &StartPageWidget::_slot_OpenManual);
	connect(ui.pushButton_Mail, &QPushButton::clicked, this, &StartPageWidget::_slot_OpenMail);
	
}

StartPageWidget::~StartPageWidget()
{
	if (m_history != nullptr) {
		delete m_history;
		m_history = nullptr;
	}

}

//显示历史创建文件信息
void StartPageWidget::_show_History()
{
	if (m_history != nullptr) {
		delete m_history;
		m_history = nullptr;
	}
	m_history = new History(this);
	m_history->get_History(m_projectname, m_projectpath, m_createtime);
	bool ismonth = false;
	bool isthismonth = false;
	bool isday = false;
	bool isyesterday = false;
	ui.listWidget_Recent->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);	
	QDateTime current_date_time = QDateTime::currentDateTime();
	m_CurrentTime = current_date_time.toString("yyyy.MM.dd hh:mm:ss");
	QString c_year = m_CurrentTime.mid(0, 4);
	QString c_day = m_CurrentTime.mid(8, 2);
	QString c_month = m_CurrentTime.mid(5, 2);
	for (int i = m_projectname.size()-1; i >= 0; i--) {
		
		QString year = m_createtime[i].mid(0, 4);
		QString month = m_createtime[i].mid(5, 2);
		QString day = m_createtime[i].mid(8, 2);
		int i_year = c_year.toInt() - year.toInt();
		int i_month = c_month.toInt() - month.toInt();
		int i_day = c_day.toInt() - day.toInt();
		if ((i_year > 0 ||i_month >= 1) && ismonth == false) {
			ui.listWidget_Recent->addItem(QString::fromLocal8Bit("更早"));
			ismonth = true;
			isthismonth = true;
			isyesterday = true;
			isday = true;
		}
		else if (i_day > 1 && isthismonth == false) {
			ui.listWidget_Recent->addItem(QString::fromLocal8Bit("本月"));
			isthismonth = true; 
			isyesterday = true;
			isday = true;
		}
		else if (i_day == 1 && isyesterday == false) {
			ui.listWidget_Recent->addItem(QString::fromLocal8Bit("昨天"));
			isyesterday = true;
			isday = true;
		}
		else if (isday == false) {
			ui.listWidget_Recent->addItem(QString::fromLocal8Bit("今天"));
			isday = true;
		}
		RecentPlugin* rp = new RecentPlugin(ui.listWidget_Recent);
		connect(rp, &RecentPlugin::signalclose, this, &StartPageWidget::_slot_delete_History);

		m_recentPlugin.push_back(rp);
		rp->set_projectName(m_projectname[i]);
		rp->set_projectPath(m_projectpath[i]);
		rp->set_ID(i);
		QListWidgetItem *recent_Item = new QListWidgetItem(ui.listWidget_Recent);
		ui.listWidget_Recent->addItem(recent_Item);
		recent_Item->setSizeHint(QSize(0, 50));
		ui.listWidget_Recent->setItemWidget(recent_Item, rp);
	}
	
}

//Label响应3D_RealTime按钮
void StartPageWidget::_slot_Describe3DRealTime()
{
	ui.label_Description->setText(QString::fromLocal8Bit("新建一个三维实时测量项目"));
}

//Label响应3Analysis按钮
void StartPageWidget::_slot_DescribeAnalysis()
{
	ui.label_Description->setText(QString::fromLocal8Bit("新建一个后处理项目"));
}

//Label回到最初的状态
void StartPageWidget::_slot_DescribeOriginal()
{
	ui.label_Description->setText(QString::fromLocal8Bit("当前未选择任何项目"));
}

//发出新建3D实时项目信号
void StartPageWidget::_slot_new3DProject()
{
	emit signalnew3DProject();
}

//发出后处理项目信号
void StartPageWidget::_slot_newAnalysis()
{
	emit signalnewAnalysis();
}

//打开操作说明书
void StartPageWidget::_slot_OpenManual()
{
	QString qtManulFile = qApp->applicationDirPath() + "/" + QString::fromLocal8Bit("说明书20200709.docx");
	QDesktopServices::openUrl(QUrl::fromLocalFile(qtManulFile));
}

//打开联系邮箱
void StartPageWidget::_slot_OpenMail()
{
	MyMessage* mes=new MyMessage("三维挠度仪测量软件 BiVSM-2020 (v1.0)",false,this);
	mes->exec();
}



void StartPageWidget::_slot_delete_History(int index)
{

	for (int i = 0; i < ui.listWidget_Recent->count(); i++)
	{
		QListWidgetItem *item = ui.listWidget_Recent->takeItem(0);
		delete item;
		i--;
	}
	for (int i = 0; i < m_recentPlugin.size(); ++i)

		m_recentPlugin.remove(i);
	
	m_history->delete_History(index);
	_show_History();
}












