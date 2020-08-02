
#include "RecentPlugin.h"

RecentPlugin::RecentPlugin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	ui.pushButton->hide();
	connect(ui.pushButton, &QPushButton::clicked, this, &RecentPlugin::send_close);
}

RecentPlugin::~RecentPlugin()
{
}

void RecentPlugin::enterEvent(QEvent * event)
{
	ui.pushButton->show();
}

void RecentPlugin::leaveEvent(QEvent * event)
{
	ui.pushButton->hide();
}

void RecentPlugin::send_close()
{
	emit signalclose(m_index);
}
