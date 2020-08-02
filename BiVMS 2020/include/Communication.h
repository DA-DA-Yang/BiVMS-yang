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
	//通信构造函数
	Communication(QString ip, qint16 port,QObject *parent=Q_NULLPTR);
	~Communication();
	//建立主站通信
	void openMaster() noexcept;
	//建立辅站通信
	void openServant() noexcept;
	//断开通信
	void breakConnect() noexcept;
	//发送给主站或辅站信号（短字符命令-定义有宏）
	void sendMessage(const char* mes) noexcept;
	//写入大数据
	void sendImageBuffer(const unsigned char* imgBuffer,int bufferSize) noexcept;
	//获取大数据
	bool getImageBuffer(unsigned char* outBuffer,int bufferSize) noexcept;
	//判断主站与辅站是否连接成功
	inline bool isConnected() noexcept { return m_connectFlag; }
	//判断本站是否为主站
	inline bool isMaster() noexcept { return m_masterFlag; }
	//返回连接端口
	inline QString port() noexcept { return m_port; }

private:
	///本类内部使用函数
	//命令发送
	void _sendMessage() noexcept;
	//获取外部信息
	void _getMessage() noexcept;
	//打开辅站通信
	void _openServant() noexcept;
	//打开主站通信
	void _openMaster() noexcept;
	//打开辅站通信
	void _breakConnect() noexcept;
	//发送大数据
	void _sendData() noexcept;
	
signals:
	///外部信号
	//通信连接成功信号
	void connected();
	//通信断开连接信号
	void disconnected();
	//打开主界面信号
	void signalOpen();
	//关闭程序信号
	void signalClose();
	//新建项目信号
	void signalNewProject();
	//加载项目信号
	void signalLoadProject();
	//打开相机信号
	void signalOpenCamera();
	//关闭相机信号
	void signalCloseCamera();
	//一键采图信号
	void signalSnapImage();
	//序列图像采集信号
	void signalCaptureImages();
	//读取完成信号
	void signalCompleted();

	///内部信号-调用多线程
	//建立辅站通信信号
	void _signalOpenServant();
	//建立主站通信信号
	void _signalOpenMaster();
	//断开通信信号
	void _signalBreakConnect();
	//发送命令信号
	void _signalSendMessage();
	//发送数据信号
	void _signalSendData();



private:
	bool                m_masterFlag{ false };          //通信连接判断，true为master，false为servant
	bool                m_connectFlag{ false };
	QThread*            m_thread = nullptr;
	QTcpServer *		m_tcpServer = nullptr;
	QTcpSocket *        m_tcpSocket = nullptr;
	QByteArray          m_writeMessage{};             //需要传出的命令
	QByteArray          m_writeData{};                //需要传出的数据
	QByteArray          m_readData{};                 //读取的数据
	QString             m_servantIP{};                //辅站ip4地址
	qint16              m_port{};                     //端口
};

