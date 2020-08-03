#include "Communication.h"
//ͨ�Ź��캯��
//QString ip ��վip4��ַ������"169.254.45.62"
//qint16 port �˿ڣ����磺8888
Communication::Communication(QString ip, qint16 port, QObject *parent)
	: QObject(parent)
{
	connect(this, &Communication::_signalOpenMaster, this, &Communication::_openMaster);
	connect(this, &Communication::_signalOpenServant, this, &Communication::_openServant);
	connect(this, &Communication::_signalBreakConnect, this, &Communication::_breakConnect);
	connect(this, &Communication::_signalSendData, this, &Communication::_sendData);
	connect(this, &Communication::_signalSendMessage, this, &Communication::_sendMessage);
	m_servantIP = ip;
	m_port = port;
	/*���߳�*/
	m_thread = new QThread(this);
	this->moveToThread(m_thread);
	m_thread->start();
	//QObject����QThread�У����źſ��ƺ�������ʵ�ֶ��߳�
}

Communication::~Communication()
{	
}
//������վͨ��
void Communication::openMaster() noexcept
{
	emit _signalOpenMaster();
}
//������վͨ��
void Communication::openServant() noexcept
{
	emit _signalOpenServant();
}
//�Ͽ�ͨ��
void Communication::breakConnect() noexcept
{
	emit _signalBreakConnect();
}
//������һ��վ��������Ϣ
//const char * mes ���ַ�����������к���"DEFINE.h"��
void Communication::sendMessage(const char * mes) noexcept
{
	m_writeMessage.clear();
	m_writeMessage.append(mes);
	emit _signalSendMessage();
}
//��������
void Communication::sendData(const QByteArray sendByteArray)
{
	m_writeData.clear();
	m_writeData.append(sendByteArray);
	emit _signalSendData();
}
//�������ݣ��ӻ������ڻ�ȡ��һ�����ݰ�
//��������ΪQByteArray����С����ǰ�趨Ϊm_dataBufferSize
//ʵ��ʹ����Ҫ��������ת��
QByteArray Communication::getData()
{
	QByteArray out;
	if (m_ByteArrayVec.size() > 0)
	{
		out.append(m_ByteArrayVec[0]);
		m_ByteArrayVec.pop_front();
	}
	return out;
}

void Communication::_sendMessage() noexcept
{
	//��������
	if (m_tcpSocket)
	{
		m_tcpSocket->write(m_writeMessage);
		m_tcpSocket->waitForBytesWritten();
	}
}

void Communication::_getMessage() noexcept
{
	//��ͨ����ȡ������
	if (m_tcpSocket)
	{
		QByteArray mes = m_tcpSocket->readAll();
		if (mes == MES_OPEN_WIDGET)
		{
			//�򿪳���
			emit signalOpen();
		}
		else if (mes == MES_EXIT)
		{
			//�رճ���
			emit signalClose();
		}
		else if (mes == MES_NEW_PROJECT)
		{
			//�½���Ŀ
			emit signalNewProject();
		}
		else if (mes == MES_LOAD_PROJECT)
		{
			//������Ŀ
			emit signalLoadProject();
		}
		else if (mes == MES_OPEN_CAMERA)
		{
			//�����
			emit signalOpenCamera();
		}
		else if (mes == MES_CLOSE_CAMERA)
		{
			//�ر����
			emit signalCloseCamera();
		}
		else if (mes == MES_SNAP_IMAGE)
		{
			//ץ��ͼ��
			emit signalSnapImage();
		}
		else if (mes == MES_CAPTURE_IMAGES)
		{
			//�ɼ�ͼ��
			emit signalCaptureImages();
		}
		else if (mes == MES_COMPLETED)
		{
			//��Ϣ�������
			emit signalCompleted();
		}
		else
		{
			m_readData.append(mes);
			QDateTime current_date_time = QDateTime::currentDateTime();
			QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
			qDebug() << "Communication data size:" << m_readData.size();
			qDebug() << "Communication time:" << current_date;
			//��ȡ����
			if (m_readData.size() >= m_dataBufferSize)
			{
				QByteArray ba;
				ba.resize(m_dataBufferSize);
				memcpy(ba.data(), m_readData, m_dataBufferSize);
				m_ByteArrayVec.push_back(ba);
				m_readData.remove(0, m_dataBufferSize);
			}
				/*emit signalCompleted();*/
		}
	}
	
}

void Communication::_openServant() noexcept
{
	//������վͨ��
	qDebug() << "communication from thread:" << QThread::currentThreadId();
	if (m_tcpSocket != nullptr)
	{
		m_tcpSocket->close();
		delete m_tcpSocket;
		m_tcpSocket = nullptr;
	}
	if (m_tcpServer)
	{
		m_tcpServer->close();
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}
	m_tcpServer = new QTcpServer(this);
	//��վ�����ź�
	m_tcpServer->listen(QHostAddress::Any, m_port);
	connect(m_tcpServer, &QTcpServer::newConnection, [=]()
		{
			//ͨ�����ӳɹ�
			m_connectFlag = true;
			emit connected();
			m_tcpSocket = m_tcpServer->nextPendingConnection();
			connect(m_tcpSocket, &QTcpSocket::disconnected, [=]()
				{
					//ͨ�ŶϿ�����
					m_connectFlag = false;
					emit disconnected();
				});
			connect(m_tcpSocket, &QTcpSocket::readyRead,
				[=]()
				{
					//��ȡ������վ����Ϣ
					_getMessage();
				});
		});
}

void Communication::_openMaster() noexcept
{
	//������վͨ��
	if (m_tcpSocket)
	{
		m_tcpSocket->close();
		delete m_tcpSocket;
		m_tcpSocket = nullptr;
	}
	if (m_tcpServer)
	{
		m_tcpServer->close();
		delete m_tcpServer;
		m_tcpServer = nullptr;
	}
	m_tcpSocket = new QTcpSocket(this);
	connect(m_tcpSocket, &QTcpSocket::connected,
		[=]()
		{
			//ͨ�����ӳɹ�
			m_connectFlag = true;
			emit connected();
			connect(m_tcpSocket, &QTcpSocket::disconnected, [=]()
				{
					//ͨ�ŶϿ�����
					m_connectFlag = false;
					emit disconnected();
				});
			connect(m_tcpSocket, &QTcpSocket::readyRead,
				[=]()
				{
					//��ȡ���Ը�վ����Ϣ
					_getMessage();
				}
			);

		}
	);
	//������վ��ǩ
	m_masterFlag = true;
	//���Ӹ�վ
	m_tcpSocket->connectToHost(QHostAddress(m_servantIP), m_port);
	m_tcpSocket->waitForConnected();
}

void Communication::_breakConnect() noexcept
{
	//�Ͽ�ͨ��
	if (m_connectFlag)
	{
		m_tcpSocket->disconnectFromHost();
		m_tcpSocket->waitForDisconnected();
	}
	if (m_thread)
	{
		m_thread->quit();
		m_thread->wait();
	}
}

void Communication::_sendData() noexcept
{
	qDebug() << "communication writeData thread:" << QThread::currentThreadId();
	//��������
	if (!m_tcpSocket)
		return;
	/*���������ķ�ʽ����*/
	QDataStream data(m_writeData);
	char dataRawArray[SIZE_MAXBYTEWRITE]{};
	quint64 writeLength{};
	while (!data.atEnd())
	{
		int length = data.readRawData(dataRawArray, SIZE_MAXBYTEWRITE);
		//QByteArray block(dataRawArray, SIZE_MAXBYTEWRITE);
		writeLength += m_tcpSocket->write(dataRawArray,length);
		qDebug() << writeLength;
		m_tcpSocket->waitForBytesWritten();
	}
}

