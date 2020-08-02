/***************************************************************************
**  BiVMS 2020                                                            **
**  A widget that displays tips                                           **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QDialog>
#include <QTimer>
#include "ui_MyMessage.h"

class MyMessage : public QDialog
{
	Q_OBJECT

public:
	MyMessage(QWidget *parent = Q_NULLPTR);
	MyMessage(const char* message,bool autoClose=true, QWidget *parent = Q_NULLPTR);
	~MyMessage();


private:
	Ui::MyMessage ui;
};
