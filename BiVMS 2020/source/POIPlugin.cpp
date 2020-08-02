#include "POIPlugin.h"

POIPlugin::POIPlugin(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	connect(ui.checkBox, &QCheckBox::clicked, this, &POIPlugin::on_checkBox_clicked);
}

POIPlugin::~POIPlugin()
{
}
