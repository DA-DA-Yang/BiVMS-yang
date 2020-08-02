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
	//发出鼠标滑过信号
	void signalHover();
	//发出鼠标移开信号
	void signalLeave();

private:
	//鼠标进入按钮响应事件
	void enterEvent(QEvent*); 
	//鼠标移除按钮响应事件
	void leaveEvent(QEvent*);        

	
};
