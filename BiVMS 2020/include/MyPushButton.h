/***************************************************************************
**  BiVMS 2020                                                            **
**  Define MyPushButton													  **

****************************************************************************
**           Author: CHENG HUANG                                          **
**          Contact: huangcheng@shu.edu.cn                                **
**             Date: 28.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QPushButton>
#include <Qevent>
class MyPushButton : public QPushButton
{
	Q_OBJECT

public:
	MyPushButton(QObject *parent);
	~MyPushButton();

signals:
	//������껬���ź�
	void signalHover();
	//��������ƿ��ź�
	void signalLeave();

private:
	//�����밴ť��Ӧ�¼�
	void enterEvent(QEvent*); 
	//����Ƴ���ť��Ӧ�¼�
	void leaveEvent(QEvent*);        

	
};
