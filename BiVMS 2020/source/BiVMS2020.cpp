#include "BiVMS2020.h"

BiVMS2020::BiVMS2020(QWidget *parent)
    : QMainWindow(parent)
{
	qDebug() << "main thread:" << QThread::currentThreadId();
	//欢迎界面
	m_startDlg = new StartDlg;
	QTimer::singleShot(1500, m_startDlg, SLOT(accept()));
	m_startDlg->exec();
	//加载主窗口
	ui.setupUi(this);
	this->resize(2000, 1200);
	_load_StartPageWidget();
	connect(ui.tabWidget, SIGNAL(tabCloseRequested(int)), this, SLOT(_remove_TabWidget(int)));
	connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(_onTabWidgetCurrentIndexChanged()));

	/*************************通信*************************/
	//命令通信端口
	m_communicationMessage = new Communication("169.254.45.62", 1111);
	m_communicationMessage->openServant();
	//图像通信端口
	m_communicationImage = new Communication("169.254.45.62", 2222);
	m_communicationImage->openServant();
	//数据通信端口
	m_communicationData = new Communication("169.254.45.62", 3333);
	m_communicationData->openServant();
	//命令通信端口与主界面的连接函数
	connect(m_communicationMessage, &Communication::signalOpen, this, &BiVMS2020::_load_RealTime3DWidget);
	connect(m_communicationMessage, &Communication::connected, this, &BiVMS2020::_onCommunicationConnected);
	connect(m_communicationMessage, &Communication::disconnected, this, &BiVMS2020::_onCommunicationDisconnected);
	connect(m_communicationMessage, &Communication::signalClose, this, &BiVMS2020::_Exit);
	//通信对话框
	CommunicationDlg* communicationDlg = new CommunicationDlg(this);

	/*************************项目*************************/
	//创建空项目
	m_project = new Project(this);
	
	/*************************状态*************************/
	//设置状态栏信息
	m_statusLabel = new QLabel(this);
	m_statusLabel->setText(QString::fromLocal8Bit("提示：尚未加载任何项目！"));
	m_statusLabel->setTextFormat(Qt::RichText);
	m_statusLabel->setOpenExternalLinks(true);
	ui.statusBar->addPermanentWidget(m_statusLabel);

	/*************************菜单*************************/
	//菜单栏消息响应函数
	connect(ui.actionRealTime3DProject, &QAction::triggered, this, &BiVMS2020::_new_RealTime3DProject);
	connect(ui.actionAnalysis3DProject, &QAction::triggered, this, &BiVMS2020::_new_Analysis3DProject);
	connect(ui.actionStartPage, &QAction::triggered, this, &BiVMS2020::_load_StartPageWidget);
	connect(ui.actionCalibration, &QAction::triggered, this, &BiVMS2020::_load_CalibrationWidget);
	connect(ui.actionHardwareControl, &QAction::triggered, this, &BiVMS2020::_load_HardwareControlWidget);
	connect(ui.actionProjectInformation, &QAction::triggered, this, &BiVMS2020::_load_ProjectInformation);
	connect(ui.actionCommuincationSetting, &QAction::triggered, this, &BiVMS2020::_onCommunicationSetting);
	connect(ui.actionExit, &QAction::triggered, this, &BiVMS2020::_Exit);
}

void BiVMS2020::closeEvent(QCloseEvent * event)
{
	if (m_communicationMessage->isConnected())
	{
		if (m_communicationMessage->isMaster())
		{
			//辅站退出信号
			MyMessage* mes = new MyMessage("确认退出吗？", false, this);
			if (mes->exec() != QDialog::Accepted)
			{
				event->ignore();
				return;
			}	
			m_communicationMessage->sendMessage(MES_EXIT);
			QTime t;
			t.start();
			while (t.elapsed() < 1000);
			m_communicationMessage->breakConnect();
		}
		else
		{
			m_communicationMessage->breakConnect();
		}
	}
	else
	{
		MyMessage* mes = new MyMessage("确认退出吗？", false, this);
		if (mes->exec() != QDialog::Accepted)
		{
			event->ignore();
			return;
		}
	}
	
}

void BiVMS2020::_remove_TabWidget(int index) noexcept
{
	ui.tabWidget->removeTab(index);
}

void BiVMS2020::_set_StatusProjectInfo(const QString filePath) noexcept
{
	m_statusLabel->setText("<a href=\"file:" + filePath + "\">" + filePath + "</a>");
}

void BiVMS2020::_new_RealTime3DProject() noexcept
{
	//新建三维实时测量项目
	m_newProjectDlg = new NewProjectDlg(this);
	m_newProjectDlg->setCommunication(m_communicationMessage, m_communicationImage, m_communicationData);
	m_newProjectDlg->copyProjectPtr(m_project);
	if (m_newProjectDlg->exec() != QDialog::Accepted)
		return;
	if (m_communicationMessage->isConnected())
	{
		m_communicationMessage->sendMessage(MES_OPEN_WIDGET);
	}
	_load_RealTime3DWidget();
}

void BiVMS2020::_new_Analysis3DProject() noexcept
{
	_load_Analysis3DWidget();
}

void BiVMS2020::_load_Project() noexcept
{
}

void BiVMS2020::_load_ProjectInformation() noexcept
{
	//添加项目信息界面
	if (m_project->isEmpty())
	{
		MyMessage* mes = new MyMessage("当前未加载任何项目！");
		return;
	}
	if (m_projectInfoDlg)
	{
		delete m_projectInfoDlg;
		m_projectInfoDlg = nullptr;
	}
	m_projectInfoDlg = new ProjectInfoDlg(m_project,this);
	m_projectInfoDlg->exec();
}

void BiVMS2020::_load_StartPageWidget() noexcept
{
	if (m_startPageWidget)
	{
		delete m_startPageWidget;
		m_startPageWidget = nullptr;
	}
	m_startPageWidget = new StartPageWidget(this);
	//起始页添加在tabwidget第一页
	ui.tabWidget->insertTab(0, m_startPageWidget,QString::fromLocal8Bit("起始页"));
	ui.tabWidget->setCurrentWidget(m_startPageWidget);
	connect(m_startPageWidget, &StartPageWidget::signalnew3DProject, this, &BiVMS2020::_new_RealTime3DProject);
	connect(m_startPageWidget, &StartPageWidget::signalnewAnalysis, this, &BiVMS2020::_new_Analysis3DProject);
}

void BiVMS2020::_load_RealTime3DWidget() noexcept
{
	//添加实时界面
	if (m_realTime3DWidget)
	{
		delete m_realTime3DWidget;
		m_realTime3DWidget = nullptr;
	}
	if (m_realTime3DToolBar)
	{
		delete m_realTime3DToolBar;
		m_realTime3DToolBar = nullptr;
	}
	//主界面添加实时界面-工具栏
	m_realTime3DToolBar = new RealTime3DToolBar(this);
	this->addToolBar(m_realTime3DToolBar);
	//主界面添加实时界面
	m_realTime3DWidget = new RealTime3DWidget(this);
	m_realTime3DWidget->setCommunication(m_communicationMessage, m_communicationImage, m_communicationData);
	if (m_communicationMessage->isConnected()&&!m_communicationMessage->isMaster())
	{
		FlagPlugin* flag = new FlagPlugin(this);
		flag->set_Flag("本站为辅站：正在通信中...");
		flag->show();
		connect(m_communicationMessage, &Communication::disconnected, flag, &FlagPlugin::close);
	}
	ui.tabWidget->addTab(m_realTime3DWidget,QString::fromLocal8Bit("实时"));
	ui.tabWidget->setCurrentWidget(m_realTime3DWidget);
	m_realTime3DWidget->setToolBar(m_realTime3DToolBar);
	//添加曲线绘图界面
	if (m_drawingDock)
	{
		delete m_drawingDock;
		m_drawingDock = nullptr;
	}
	m_drawingDock = new DrawingDock(this);
	addDockWidget(Qt::RightDockWidgetArea, m_drawingDock);
	m_realTime3DWidget->setDrawingDock(m_drawingDock);
}

void BiVMS2020::_load_Analysis3DWidget() noexcept
{
	//添加分析界面
	if (widget3)
	{
		delete widget3;
		widget3 = nullptr;
	}
	widget3 = new QWidget(this);
	QGridLayout* layout = new QGridLayout(widget3);
	QLabel* label = new QLabel(widget3);
	label->setStyleSheet("QLabel{font: 40pt;color: rgb(0, 0, 0); }");
	label->setText(QString::fromLocal8Bit("这里是图像分析界面"));
	layout->addWidget(label);
	widget3->setLayout(layout);
	ui.tabWidget->addTab(widget3, QString::fromLocal8Bit("分析"));
	ui.tabWidget->setCurrentWidget(widget3);
}

void BiVMS2020::_load_CalibrationWidget() noexcept
{
	if (widget)
	{
		delete widget;
		widget = nullptr;
	}
	widget = new QWidget(this);
	QGridLayout* layout=new QGridLayout(widget);
	QLabel* label = new QLabel(widget);
	label->setStyleSheet("QLabel{font: 40pt;color: rgb(0, 0, 0); }");
	label->setText(QString::fromLocal8Bit("这里是标定界面"));
	layout->addWidget(label);
	widget->setLayout(layout);
	ui.tabWidget->addTab(widget,QString::fromLocal8Bit("标定"));
	ui.tabWidget->setCurrentWidget(widget);
}

void BiVMS2020::_load_HardwareControlWidget() noexcept
{
	if (widget2)
	{
		delete widget2;
		widget2 = nullptr;
	}
	widget2 = new QWidget(this);
	QGridLayout* layout = new QGridLayout(widget2);
	QLabel* label = new QLabel(widget2);
	label->setStyleSheet("QLabel{font: 40pt;color: rgb(0, 0, 0); }");
	label->setText(QString::fromLocal8Bit("这里是转台、倾角仪、测距仪控制界面"));
	layout->addWidget(label);
	widget2->setLayout(layout);
	ui.tabWidget->addTab(widget2, QString::fromLocal8Bit("硬件控制"));
	ui.tabWidget->setCurrentWidget(widget2);
}

void BiVMS2020::_Exit() noexcept
{
	//退出主程序
	this->close();
	
}

void BiVMS2020::_onCommunicationSetting() noexcept
{
	CommunicationDlg* dlg = new CommunicationDlg(this);
	dlg->setCommunication(m_communicationMessage, m_communicationImage, m_communicationData);
	dlg->show();
}

void BiVMS2020::_onCommunicationConnected() noexcept
{
	if (m_communicationMessage->isMaster())
	{
		MyMessage* mes = new MyMessage("成功与辅站建立通信！");
	}
	else
	{
		MyMessage* mes = new MyMessage("成功与主站建立通信！");
	}
}

void BiVMS2020::_onCommunicationDisconnected() noexcept
{
	if (m_communicationMessage->isMaster())
	{
		MyMessage* mes = new MyMessage("与辅站断开通信！");
	}
	else
	{
		MyMessage* mes = new MyMessage("与主站断开通信，等待主站重新连接！");
	}
}

void BiVMS2020::_onTabWidgetCurrentIndexChanged() noexcept
{
	if (ui.tabWidget->currentWidget() == m_realTime3DWidget)
	{
		if (m_drawingDock)
		{
			m_drawingDock->setVisible(true);
		}
		if (m_realTime3DToolBar)
		{
			m_realTime3DToolBar->setVisible(true);
		}
		
	}
	else
	{
		if (m_drawingDock)
		{
			m_drawingDock->setVisible(false);
		}
		if (m_realTime3DToolBar)
		{
			m_realTime3DToolBar->setVisible(false);
		}
	}
}
