/***************************************************************************
**  BiVMS 2020                                                            **
**  A class for Wired connection communication                            **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QObject>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#include <QThread>
#include "MyMessage.h"
#include "DEFINE.h"

class Communication : public QObject
{
	Q_OBJECT

public:
	//ͨ�Ź��캯��
	Communication(QString ip, qint16 port,QObject *parent=Q_NULLPTR);
	~Communication();
	//������վͨ��
	void openMaster() noexcept;
	//������վͨ��
	void openServant() noexcept;
	//�Ͽ�ͨ��
	void breakConnect() noexcept;
	//���͸���վ��վ�źţ����ַ�����-�����к꣩
	void sendMessage(const char* mes) noexcept;
	//д�������
	void sendImageBuffer(const unsigned char* imgBuffer,int bufferSize) noexcept;
	//��ȡ������
	bool getImageBuffer(unsigned char* outBuffer,int bufferSize) noexcept;
	//�ж���վ�븨վ�Ƿ����ӳɹ�
	inline bool isConnected() noexcept { return m_connectFlag; }
	//�жϱ�վ�Ƿ�Ϊ��վ
	inline bool isMaster() noexcept { return m_masterFlag; }
	//�������Ӷ˿�
	inline QString port() noexcept { return m_port; }

private:
	///�����ڲ�ʹ�ú���
	//�����
	void _sendMessage() noexcept;
	//��ȡ�ⲿ��Ϣ
	void _getMessage() noexcept;
	//�򿪸�վͨ��
	void _openServant() noexcept;
	//����վͨ��
	void _openMaster() noexcept;
	//�򿪸�վͨ��
	void _breakConnect() noexcept;
	//���ʹ�����
	void _sendData() noexcept;
	
signals:
	///�ⲿ�ź�
	//ͨ�����ӳɹ��ź�
	void connected();
	//ͨ�ŶϿ������ź�
	void disconnected();
	//���������ź�
	void signalOpen();
	//�رճ����ź�
	void signalClose();
	//�½���Ŀ�ź�
	void signalNewProject();
	//������Ŀ�ź�
	void signalLoadProject();
	//������ź�
	void signalOpenCamera();
	//�ر�����ź�
	void signalCloseCamera();
	//һ����ͼ�ź�
	void signalSnapImage();
	//����ͼ��ɼ��ź�
	void signalCaptureImages();
	//��ȡ����ź�
	void signalCompleted();

	///�ڲ��ź�-���ö��߳�
	//������վͨ���ź�
	void _signalOpenServant();
	//������վͨ���ź�
	void _signalOpenMaster();
	//�Ͽ�ͨ���ź�
	void _signalBreakConnect();
	//���������ź�
	void _signalSendMessage();
	//���������ź�
	void _signalSendData();



private:
	bool                m_masterFlag{ false };          //ͨ�������жϣ�trueΪmaster��falseΪservant
	bool                m_connectFlag{ false };
	QThread*            m_thread = nullptr;
	QTcpServer *		m_tcpServer = nullptr;
	QTcpSocket *        m_tcpSocket = nullptr;
	QByteArray          m_writeMessage{};             //��Ҫ����������
	QByteArray          m_writeData{};                //��Ҫ����������
	QByteArray          m_readData{};                 //��ȡ������
	QString             m_servantIP{};                //��վip4��ַ
	qint16              m_port{};                     //�˿�
};

