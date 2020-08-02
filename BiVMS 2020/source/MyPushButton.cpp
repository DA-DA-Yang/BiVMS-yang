#include "MyPushButton.h"

MyPushButton::MyPushButton(QObject *parent)
	: QPushButton()
{
}

MyPushButton::~MyPushButton()
{
}

//鼠标进入按钮响应事件
void MyPushButton::enterEvent(QEvent *)
{
	emit signalHover();
}

//鼠标移除按钮响应事件
void MyPushButton::leaveEvent(QEvent *)
{
	emit signalLeave();
}
