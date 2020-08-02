#pragma once

#include <QDialog>
#include "ui_CommunicationDlg.h"
#include "Communication.h"

class CommunicationDlg : public QDialog
{
	Q_OBJECT

public:
	CommunicationDlg(QWidget *parent = Q_NULLPTR);
	~CommunicationDlg();
	//命令及图像传输通信端
	inline void setCommunicationMessage(Communication* communication) noexcept {
		m_communicationMessage = communication;
		set_statusMessage();
		connect(m_communicationMessage, &Communication::connected, this, &CommunicationDlg::set_statusMessage);
	}
	//测点数据传输通信端
	inline void setCommunicationData(Communication* communication) noexcept {
		m_communicationData = communication;
		set_statusData();
		connect(m_communicationData, &Communication::connected, this, &CommunicationDlg::set_statusData);
	}
private slots:
	void on_pBt_ConnectMessage();
	void on_pBt_ConnectData();
	void set_statusMessage();
	void set_statusData();
private:
	Ui::CommunicationDlg ui;
	Communication* m_communicationMessage = nullptr;
	Communication* m_communicationData = nullptr;
};
