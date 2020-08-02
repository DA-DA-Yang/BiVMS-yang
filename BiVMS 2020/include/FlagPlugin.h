/***************************************************************************
**  BiVMS 2020                                                            **
**  A plugin for showWidget                                               **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QWidget>
#include "ui_FlagPlugin.h"

class FlagPlugin : public QWidget
{
	Q_OBJECT

public:
	FlagPlugin(QWidget *parent = Q_NULLPTR);
	~FlagPlugin();
	inline void set_Flag(const char* flag) noexcept { ui.label->setText(QString::fromLocal8Bit(flag)); }

private:
	Ui::FlagPlugin ui;
};
