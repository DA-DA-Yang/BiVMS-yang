#include "CommunicationDlg.h"

CommunicationDlg::CommunicationDlg(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	connect(ui.pushButton_Message, &QPushButton::clicked, this, &CommunicationDlg::on_pBt_ConnectMessage);
	connect(ui.pushButton_Image, &QPushButton::clicked, this, &CommunicationDlg::on_pBt_ConnectImage);
	connect(ui.pushButton_Data, &QPushButton::clicked, this, &CommunicationDlg::on_pBt_ConnectData);

}

CommunicationDlg::~CommunicationDlg()
{
}

void CommunicationDlg::on_pBt_ConnectMessage()
{
	if (m_communicationMessage)
	{
		if (!m_communicationMessage->isConnected())
		{
			m_communicationMessage->openMaster();
		}
	}
}

void CommunicationDlg::on_pBt_ConnectImage()
{
	if (m_communicationImage)
	{
		if (!m_communicationImage->isConnected())
		{
			m_communicationImage->openMaster();
		}
	}
}

void CommunicationDlg::on_pBt_ConnectData()
{
	if (m_communicationData)
	{
		if (!m_communicationData->isConnected())
		{
			m_communicationData->openMaster();
		}
	}
}

void CommunicationDlg::set_statusMessage()
{
	if (m_communicationMessage)
	{
		if (m_communicationMessage->isConnected())
		{
			ui.label_statusMessage->setText(QString::fromLocal8Bit("已连接"));
		}
		else
		{
			ui.label_statusMessage->setText(QString::fromLocal8Bit("未连接"));
		}
	}
	else
	{
		ui.label_statusMessage->setText(QString::fromLocal8Bit("错误"));
	}
}

void CommunicationDlg::set_statusImage()
{
	if (m_communicationImage)
	{
		if (m_communicationImage->isConnected())
		{
			ui.label_statusImage->setText(QString::fromLocal8Bit("已连接"));
		}
		else
		{
			ui.label_statusImage->setText(QString::fromLocal8Bit("未连接"));
		}
	}
	else
	{
		ui.label_statusImage->setText(QString::fromLocal8Bit("错误"));
	}
}

void CommunicationDlg::set_statusData()
{
	if (m_communicationData)
	{
		if (m_communicationData->isConnected())
		{
			ui.label_statusData->setText(QString::fromLocal8Bit("已连接"));
		}
		else
		{
			ui.label_statusData->setText(QString::fromLocal8Bit("未连接"));
		}
	}
	else
	{
		ui.label_statusData->setText(QString::fromLocal8Bit("错误"));
	}
}
