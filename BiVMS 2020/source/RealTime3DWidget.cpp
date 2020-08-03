#include "RealTime3DWidget.h"

RealTime3DWidget::RealTime3DWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	for (int i = 0; i < 256; i++)
	{
		m_grayColourTable.append(qRgb(i, i, i));
	}
}

RealTime3DWidget::~RealTime3DWidget()
{
	_closeCamera();
}
//�����
void RealTime3DWidget::_openCamera() noexcept
{
	//�����

	//���������ʾ
	if (m_showMaster)
	{
		delete m_showMaster;
		m_showMaster = nullptr;
	}
	if (m_showServant)
	{
		delete m_showServant;
		m_showServant = nullptr;
	}
	if (m_communicationMessage)
	{
		if (m_communicationMessage->isConnected())
		{
			if (m_communicationMessage->isMaster())
			{
				//��վ������ʾ��ͼ
				m_showMaster = new ShowWidget(this);
				FlagPlugin* flag_master = new FlagPlugin(m_showMaster);
				flag_master->set_Flag("��վ");
				ui.horizontalLayout->addWidget(m_showMaster);
				//��վ�и�վ������ʾ��ͼ
				m_showServant = new ShowWidget(this);
				FlagPlugin* flag_servant = new FlagPlugin(m_showServant);
				flag_servant->set_Flag("��վ");
				ui.horizontalLayout->addWidget(m_showServant);
				//��ͼ��--ʹ�����ʱɾ��������
				m_masterImg.load("1.bmp");
				m_servantImg.load("1.bmp");
				m_communicationImage->setDataBufferSize(m_servantImg.byteCount());
				//���͸�վ������ź�
				m_communicationMessage->sendMessage(MES_OPEN_CAMERA);
			}
			else
			{
				//��վ������ʾ
				m_showMaster = new ShowWidget(this);
				FlagPlugin* flag_servant = new FlagPlugin(m_showMaster);
				flag_servant->set_Flag("��վ");
				ui.horizontalLayout->addWidget(m_showMaster);
				//��ͼ��--ʹ�����ʱɾ��������
				m_masterImg.load("2.bmp");
			}
		}
		else
		{
			//�������ӣ�ֻ��ʾ��վ����
			m_showMaster = new ShowWidget(this);
			ui.horizontalLayout->addWidget(m_showMaster);
			m_masterImg.load("1.bmp");
		}
	}
	else
	{
		//�������ӣ�ֻ��ʾ��վ����
		m_showMaster = new ShowWidget(this);
		ui.horizontalLayout->addWidget(m_showMaster);
		//��ͼ��--ʹ�����ʱɾ��������
		m_masterImg.load("1.bmp");
	}
	//toolBarֻ��Ӧ��վ����
	connect(m_toolBar->UI().actionAddPoint, &QAction::triggered, m_showMaster, &ShowWidget::add_Point);
	connect(m_toolBar->UI().actionDeletePoint, &QAction::triggered, m_showMaster, &ShowWidget::delete_Point);
	connect(m_toolBar->UI().actionClearPoint, &QAction::triggered, m_showMaster, &ShowWidget::clear_Point);
	connect(m_showMaster, &ShowWidget::signal_AddPoint, this, &RealTime3DWidget::_slotAddPOI);
	connect(m_showMaster, &ShowWidget::signal_DeletePoint, this, &RealTime3DWidget::_slotDeletePOI);
	connect(m_showMaster, &ShowWidget::signal_ClearPoint, this, &RealTime3DWidget::_slotClearPOI);
	if (m_timer)
	{
		delete m_timer;
		m_timer = nullptr;
	}
	//��ʱ����������ͼ��ˢ��
	m_timer = new QTimer(this);
	m_timer->setInterval(20);
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &RealTime3DWidget::_updateShow);
	m_timeMasterShow.start();
	m_timeServantShow.start();
	m_computeTime.start();
	return;
}

void RealTime3DWidget::_closeCamera() noexcept
{
	//��ֹͣ��ʱ��
	m_timer->stop();
	//���͸�վ�ر�����ź�
	if (m_communicationMessage)
		if (m_communicationMessage->isConnected())
			if (m_communicationMessage->isMaster())
				m_communicationMessage->sendMessage(MES_CLOSE_CAMERA);
	//�ȴ�һ��ʱ��
	QTime t;
	t.start();
	while (t.elapsed() < 100);

	/*******�ڴ��������رպ���******/

	//ɾ����ʾ��ͼ
	if (m_showMaster)
	{
		delete m_showMaster;
		m_showMaster = nullptr;
		m_masterImg = QImage();
	}
	if (m_showServant)
	{
		delete m_showServant;
		m_showServant = nullptr;
		m_servantImg = QImage();
	}
}

//��վͼ����ʾ���ɶ�ʱ������
void RealTime3DWidget::_updateShow() noexcept
{
	//������ʾ֡��
	if (m_timeMasterShow.elapsed() >= 1000)
	{
		m_timeMasterShow.restart();
		m_showMaster->showFPS(m_FPSMasterShow);
		m_FPSMasterShow = 0;
	}
	m_FPSMasterShow++;
	if (m_masterImg.bits())
		m_showMaster->showImage(m_masterImg);
	if (m_communicationImage)
	{
		if (m_communicationImage->isConnected())
		{
			if (m_communicationImage->isMaster())
			{
				//��վ���ո�վͼ����ʾ
				_updateServantShow();
				
			}
			else
			{
				//��վ����ͼ��
				_sendServantImg();
				//��վ���Ͳ������
				_sendServantData();
			}
		}	
	}
		
	
}
//��վ��-��վͼ����ʾ����ͨ�ſ���
void RealTime3DWidget::_updateServantShow() noexcept
{
	//��ȡ��վ��ͼ��
	if (m_servantImg.bits())
	{
		auto byteArrayImage= m_communicationImage->getData();
		if (byteArrayImage.size()== m_servantImg.byteCount())
		{
			memcpy(m_servantImg.bits(), byteArrayImage, m_servantImg.byteCount());
			m_showServant->showImage(m_servantImg);
			//������ʾ֡��
			if (m_timeServantShow.elapsed() >= 1000)
			{
				m_timeServantShow.restart();
				m_showServant->showFPS(m_FPSServantShow);
				m_FPSServantShow = 0;
			}
			m_FPSServantShow++;
		}
		auto byteArrayData = m_communicationData->getData();
		m_showServant->showTime((QString)byteArrayData);
	}
	
}

void RealTime3DWidget::_startCompute() noexcept
{
	if (!m_POIs.size())
	{
		MyMessage* mes = new MyMessage("��δ��Ӳ����㣡");
		return;
	}
	m_computing = true;
	m_drawingDock->show(m_POIs.size());
}

void RealTime3DWidget::_finishCompute() noexcept
{
	m_computing = false;
	m_drawingDock->close();
}

//��վ����ͼ��
void RealTime3DWidget::_sendServantImg() noexcept
{
	if (m_masterImg.bits())
	{
		QByteArray ba;
		ba.resize(m_masterImg.byteCount());
		memcpy(ba.data(), m_masterImg.bits(), m_masterImg.byteCount());
		m_communicationImage->sendData(ba);
	}
}

//��վ���Ͳ������
void RealTime3DWidget::_sendServantData() noexcept
{
	/**************Demo**************/
	//��ȡ��ǰʱ��
	QDateTime current_date_time = QDateTime::currentDateTime();
	QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
	QByteArray ba;
	ba.append(current_date);
	if (m_communicationData)
	{
		m_communicationData->sendData(ba);
	}
}


void RealTime3DWidget::_slotAddPOI() noexcept
{
	//���ѡ����POI
	auto point_in_scene = m_showMaster->scene()->m_seeds.back();
	auto index = m_showMaster->scene()->m_seeds.size();
	m_POIs.push_back({ (int)point_in_scene->x(),(int)point_in_scene->y() });
	m_drawingDock->add_POIPlugin();
}

void RealTime3DWidget::_slotDeletePOI(int index) noexcept
{
	m_POIs.removeAt(index);
	m_drawingDock->delete_POIPlugin(index);
}

void RealTime3DWidget::_slotClearPOI() noexcept
{
	m_POIs.clear();
	m_drawingDock->clear_POIPlugin();
}
