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
	//����ͨ�ţ��ý�����Ҫ��������ͨ�Ŷ˿�
	inline void setCommunication(Communication* cMessage, Communication* cImage, Communication* cData) noexcept
	{
		m_communicationMessage = cMessage;
		m_communicationImage = cImage;
		m_communicationData = cData;
		//ע������ͨ�Ŷ˿����Ӷ���
		set_statusMessage();
		connect(m_communicationMessage, &Communication::connected, this, &CommunicationDlg::set_statusMessage);
		set_statusImage();
		connect(m_communicationImage, &Communication::connected, this, &CommunicationDlg::set_statusImage);
		set_statusData();
		connect(m_communicationData, &Communication::connected, this, &CommunicationDlg::set_statusData);

	}
private slots:
	void on_pBt_ConnectMessage();
	void on_pBt_ConnectImage();
	void on_pBt_ConnectData();
	void set_statusMessage();
	void set_statusImage();
	void set_statusData();
private:
	Ui::CommunicationDlg ui;
	Communication* m_communicationMessage = nullptr;
	Communication* m_communicationImage = nullptr;
	Communication* m_communicationData = nullptr;
};
