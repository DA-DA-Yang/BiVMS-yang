#include "Communication.h"
//通信构造函数
//QString ip 辅站ip4地址，常用"169.254.45.62"
//qint16 port 端口，例如：8888
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
	/*多线程*/
	m_thread = new QThread(this);
	this->moveToThread(m_thread);
	m_thread->start();
	//QObject放在QThread中，由信号控制函数调用实现多线程
}

Communication::~Communication()
{	
}
//建立主站通信
void Communication::openMaster() noexcept
{
	emit _signalOpenMaster();
}
//建立辅站通信
void Communication::openServant() noexcept
{
	emit _signalOpenServant();
}
//断开通信
void Communication::breakConnect() noexcept
{
	emit _signalBreakConnect();
}
//发送另一个站的命令消息
//const char * mes 短字符串命令，定义有宏在"DEFINE.h"中
void Communication::sendMessage(const char * mes) noexcept
{
	m_writeMessage.clear();
	m_writeMessage.append(mes);
	emit _signalSendMessage();
}
//发送数据
void Communication::sendData(const QByteArray sendByteArray)
{
	m_writeData.clear();
	m_writeData.append(sendByteArray);
	emit _signalSendData();
}
//接收数据，从缓冲区内获取第一个数据包
//数据类型为QByteArray，大小已提前设定为m_dataBufferSize
//实际使用需要数据类型转换
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
	//发送命令
	if (m_tcpSocket)
	{
		m_tcpSocket->write(m_writeMessage);
		m_tcpSocket->waitForBytesWritten();
	}
}

void Communication::_getMessage() noexcept
{
	//从通信中取出内容
	if (m_tcpSocket)
	{
		QByteArray mes = m_tcpSocket->readAll();
		if (mes == MES_OPEN_WIDGET)
		{
			//打开程序
			emit signalOpen();
		}
		else if (mes == MES_EXIT)
		{
			//关闭程序
			emit signalClose();
		}
		else if (mes == MES_NEW_PROJECT)
		{
			//新建项目
			emit signalNewProject();
		}
		else if (mes == MES_LOAD_PROJECT)
		{
			//加载项目
			emit signalLoadProject();
		}
		else if (mes == MES_OPEN_CAMERA)
		{
			//打开相机
			emit signalOpenCamera();
		}
		else if (mes == MES_CLOSE_CAMERA)
		{
			//关闭相机
			emit signalCloseCamera();
		}
		else if (mes == MES_SNAP_IMAGE)
		{
			//抓拍图像
			emit signalSnapImage();
		}
		else if (mes == MES_CAPTURE_IMAGES)
		{
			//采集图像
			emit signalCaptureImages();
		}
		else if (mes == MES_COMPLETED)
		{
			//信息发送完成
			emit signalCompleted();
		}
		else
		{
			m_readData.append(mes);
			QDateTime current_date_time = QDateTime::currentDateTime();
			QString current_date = current_date_time.toString("yyyy.MM.dd hh:mm:ss.zzz");
			qDebug() << "Communication data size:" << m_readData.size();
			qDebug() << "Communication time:" << current_date;
			//提取数据
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
	//建立辅站通信
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
	//辅站监听信号
	m_tcpServer->listen(QHostAddress::Any, m_port);
	connect(m_tcpServer, &QTcpServer::newConnection, [=]()
		{
			//通信连接成功
			m_connectFlag = true;
			emit connected();
			m_tcpSocket = m_tcpServer->nextPendingConnection();
			connect(m_tcpSocket, &QTcpSocket::disconnected, [=]()
				{
					//通信断开连接
					m_connectFlag = false;
					emit disconnected();
				});
			connect(m_tcpSocket, &QTcpSocket::readyRead,
				[=]()
				{
					//获取来自主站的信息
					_getMessage();
				});
		});
}

void Communication::_openMaster() noexcept
{
	//建立主站通信
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
			//通信连接成功
			m_connectFlag = true;
			emit connected();
			connect(m_tcpSocket, &QTcpSocket::disconnected, [=]()
				{
					//通信断开连接
					m_connectFlag = false;
					emit disconnected();
				});
			connect(m_tcpSocket, &QTcpSocket::readyRead,
				[=]()
				{
					//获取来自辅站的信息
					_getMessage();
				}
			);

		}
	);
	//设置主站标签
	m_masterFlag = true;
	//连接辅站
	m_tcpSocket->connectToHost(QHostAddress(m_servantIP), m_port);
	m_tcpSocket->waitForConnected();
}

void Communication::_breakConnect() noexcept
{
	//断开通信
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
	//发送数据
	if (!m_tcpSocket)
		return;
	/*用数据流的方式发送*/
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

