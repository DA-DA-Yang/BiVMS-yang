#include "DrawingDock.h"


DrawingDock::DrawingDock(QWidget *parent)
	: QDockWidget(parent)
{
	ui.setupUi(this);
	setFeatures(QDockWidget::AllDockWidgetFeatures);
	//显示xyz三条曲线
	m_drawWidget_X= new DrawWidget(this);
	m_drawWidget_X->setLabels("测量时间（s) ", "X位移（mm）");
	m_drawWidget_Y= new DrawWidget(this);
	m_drawWidget_Y->setLabels("测量时间（s) ", "Y位移（mm）");
	m_drawWidget_Z= new DrawWidget(this);
	m_drawWidget_Z->setLabels("测量时间（s) ", "Z位移（mm）");
	ui.gridLayout->addWidget(m_drawWidget_X, 0, 0);
	ui.gridLayout->addWidget(m_drawWidget_Y, 1, 0);
	ui.gridLayout->addWidget(m_drawWidget_Z, 2, 0);
	
	
	ui.widget_setting->setVisible(false);
	connect(ui.pushButton_setting, &QPushButton::clicked, this, &DrawingDock::on_pBt_setting);
	connect(ui.checkBox_showX, &QCheckBox::clicked, this, &DrawingDock::on_checkBox_showX);
	connect(ui.checkBox_showY, &QCheckBox::clicked, this, &DrawingDock::on_checkBox_showY);
	connect(ui.checkBox_showZ, &QCheckBox::clicked, this, &DrawingDock::on_checkBox_showZ);

	m_timer = new QTimer(this);
	m_timer->setInterval(30);
	//m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &DrawingDock::_updateShow);

	ui.listWidget->setStyleSheet(
		"QListWidget{border:1px solid gray; color:black; }"
		"QListWidget::Item{border-width:0px }"
		"QListWidget::Item:hover{background:skyblue; }"
		"QListWidget::item:selected{background:lightgray; color:red; }"
		"QListWidget::item:selected:!active{border-width:0px; background:lightgreen; }"
	);
	m_POIPlugin_first = new POIPlugin(ui.listWidget);
	QListWidgetItem *poi_Item = new QListWidgetItem(ui.listWidget);
	ui.listWidget->addItem(poi_Item);
	poi_Item->setSizeHint(QSize(0, 50));
	ui.listWidget->setItemWidget(poi_Item, m_POIPlugin_first);
	connect(m_POIPlugin_first, &POIPlugin::signal_CheckChanged, this, &DrawingDock::_selected_AllPOIPlugins);
}

DrawingDock::~DrawingDock()
{
}

void DrawingDock::addPOIPlugin() noexcept
{
	POIPlugin* poi_plugin = new POIPlugin(ui.listWidget);
	m_POIPlugins.push_back(poi_plugin);
	poi_plugin->set_ID(m_POIPlugins.size());
	poi_plugin->setXValue(0.0);
	poi_plugin->setYValue(0.0);
	poi_plugin->setZValue(0.0);

	QListWidgetItem *poi_Item = new QListWidgetItem(ui.listWidget);
	ui.listWidget->addItem(poi_Item);
	poi_Item->setSizeHint(QSize(0, 50));
	ui.listWidget->setItemWidget(poi_Item, poi_plugin);
}

void DrawingDock::deletePOIPlugin(const int & index) noexcept
{
	for (int i = index+1; i < m_POIPlugins.size(); ++i)
	{
		m_POIPlugins[i]->set_ID(i);
	}
	ui.listWidget->removeItemWidget(ui.listWidget->item(index+1));
	delete ui.listWidget->item(index+1);
	m_POIPlugins.removeAt(index);
	ui.listWidget->update();
}

void DrawingDock::clearPOIPlugin() noexcept
{

	for (int i = 0; i < ui.listWidget->count()-1; ++i)
	{
		ui.listWidget->removeItemWidget(ui.listWidget->item(i+1));
		delete ui.listWidget->item(i+1);
		m_POIPlugins.removeAt(i);
		i--;
	}
	ui.listWidget->update();
}

void DrawingDock::setPOIValue(const int & index, const double & xValue, const double & yValue, const double & zValue)
{
	//设置POI插件的数值显示
	m_POIPlugins[index]->setXValue(xValue);
	m_POIPlugins[index]->setYValue(yValue);
	m_POIPlugins[index]->setZValue(zValue);
}

void DrawingDock::setData(QVector<double> time, QVector<double> xData, QVector<double> yData, QVector<double> zData) noexcept
{
	//获取计算结果用以显示
	//显示分两种--曲线与插件
	m_data_Time = time;
	m_data_X = xData;
	m_data_Y = yData;
	m_data_Z = zData;
	_updateShow();
}

void DrawingDock::setXRange()
{
	m_drawWidget_X->setXRange(m_data_Time.back(), m_number, Qt::AlignRight);
	m_drawWidget_Y->setXRange(m_data_Time.back(), m_number, Qt::AlignRight);
	m_drawWidget_Z->setXRange(m_data_Time.back(), m_number, Qt::AlignRight);
}

void DrawingDock::setYRange()
{

}

void DrawingDock::_updateShow() noexcept
{
	if (!m_linesCount)
		return;
	//显示时长
	switch (ui.comboBox_time->currentIndex())
	{
	case 0:
	{
		m_number = ui.spinBox_count->value();
		break;
	}
	case 1:
	{
		m_number = ui.spinBox_count->value() * 60;
		break;
	}
	case 2:
	{
		m_number = ui.spinBox_count->value() * 3600;
		break;
	}
	default:
		break;
	}
	m_drawWidget_X->addData(m_data_Time.back(), m_data_X);
	m_drawWidget_Y->addData(m_data_Time.back(), m_data_Y);
	m_drawWidget_Z->addData(m_data_Time.back(), m_data_Z);
	m_drawWidget_X->setXRange(m_data_Time.back(), m_number, Qt::AlignRight);
	m_drawWidget_Y->setXRange(m_data_Time.back(), m_number, Qt::AlignRight);
	m_drawWidget_Z->setXRange(m_data_Time.back(), m_number, Qt::AlignRight);
	if (m_POIPlugins.size() == m_data_X.size())
	{
		for (int i = 0; i < m_POIPlugins.size(); ++i)
		{
			setPOIValue(i,m_data_X[i], m_data_Y[i], m_data_Z[i]);
		}
	}
}


void DrawingDock::_selected_AllPOIPlugins(bool checked) noexcept
{
	for (int i = 0; i < m_POIPlugins.size(); ++i)
	{
		m_POIPlugins[i]->set_Checked(checked);
	}
}

void DrawingDock::on_checkBox_showX() noexcept
{
	if (ui.checkBox_showX->isChecked())
	{
		m_drawWidget_X->setVisible(true);
	}
	else
	{
		m_drawWidget_X->setVisible(false);
	}
}

void DrawingDock::on_checkBox_showY() noexcept
{
	if (ui.checkBox_showY->isChecked())
	{
		m_drawWidget_Y->setVisible(true);
	}
	else
	{
		m_drawWidget_Y->setVisible(false);
	}
}

void DrawingDock::on_checkBox_showZ() noexcept
{
	if (ui.checkBox_showZ->isChecked())
	{
		m_drawWidget_Z->setVisible(true);
	}
	else
	{
		m_drawWidget_Z->setVisible(false);
	}
}

void DrawingDock::on_pBt_setting() noexcept
{
	if (ui.widget_setting->isVisible())
	{
		ui.widget_setting->setVisible(false);
	}
	else
	{
		ui.widget_setting->setVisible(true);
	}
	
	
}

