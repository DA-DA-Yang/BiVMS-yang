#include "MyPushButton.h"

MyPushButton::MyPushButton(QObject *parent)
	: QPushButton()
{
}

MyPushButton::~MyPushButton()
{
}

//�����밴ť��Ӧ�¼�
void MyPushButton::enterEvent(QEvent *)
{
	emit signalHover();
}

//����Ƴ���ť��Ӧ�¼�
void MyPushButton::leaveEvent(QEvent *)
{
	emit signalLeave();
}
