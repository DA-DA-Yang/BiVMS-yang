/***************************************************************************
**  BiVMS 2020                                                            **
**  A dialog that displays welcome information                            **

****************************************************************************
**           Author: CHENG HUANG & DA YANG                                **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QDialog>
#include "ui_StartDlg.h"

class StartDlg : public QDialog
{
	Q_OBJECT

public:
	StartDlg(QWidget *parent = Q_NULLPTR);
	~StartDlg();
	inline Ui::StartDlg UI() noexcept { return ui; }

private:
	Ui::StartDlg ui;
};
