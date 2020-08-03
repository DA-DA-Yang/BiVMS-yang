/***************************************************************************
**  BiVMS 2020                                                            **
**  A plug-in that displays measuring point information                   **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once
#include <QWidget>
#include "ui_POIPlugin.h"

class POIPlugin : public QWidget
{
	Q_OBJECT

public:
	POIPlugin(QWidget *parent = Q_NULLPTR);
	~POIPlugin();
	inline void set_ID(int index) noexcept { ui.label_ID->setText(QString::fromLocal8Bit("²âµã") + QString::number(index)); }
	inline void setXValue(double value) noexcept { ui.label_XValue->setText(QString::number(value,'f',3)); }
	inline void setYValue(double value) noexcept { ui.label_YValue->setText(QString::number(value,'f',3)); }
	inline void setZValue(double value) noexcept { ui.label_ZValue->setText(QString::number(value,'f',3)); }
	inline void set_Checked(bool checked) noexcept { ui.checkBox->setChecked(checked); }
private slots:
	inline void on_checkBox_clicked() noexcept { emit signal_CheckChanged(ui.checkBox->isChecked()); }
signals:
	void signal_CheckChanged(bool checked);

private:
	Ui::POIPlugin ui;
};
