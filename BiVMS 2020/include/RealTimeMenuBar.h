#pragma once

#include <QMenuBar>
#include "ui_RealTimeMenuBar.h"

class RealTimeMenuBar : public QMenuBar
{
	Q_OBJECT

public:
	RealTimeMenuBar(QWidget *parent = Q_NULLPTR);
	~RealTimeMenuBar();

	inline Ui::RealTimeMenuBar UI() noexcept { return ui; }

private:
	Ui::RealTimeMenuBar ui;
};
