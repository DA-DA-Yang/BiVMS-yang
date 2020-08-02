/***************************************************************************
**  BiVMS 2020                                                            **
**  A toolBar for 3D-realtime widget       		                          **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QToolBar>
#include "ui_RealTime3DToolBar.h"

class RealTime3DToolBar : public QToolBar
{
	Q_OBJECT

public:
	RealTime3DToolBar(QWidget *parent = Q_NULLPTR);
	~RealTime3DToolBar();
	inline const Ui::RealTime3DToolBar UI() noexcept { return ui; }
	
private:
	//void _update_showImage() noexcept;

private:
	Ui::RealTime3DToolBar ui;
};
