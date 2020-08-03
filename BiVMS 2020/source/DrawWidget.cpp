#include "DrawWidget.h"

DrawWidget::DrawWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	
	initColor();
	/*******************曲线绘图区初始化设置*********************/
	QFont fontLabel,fontTick;
	fontLabel.setFamily("Microsoft YaHei");
	fontLabel.setPointSize(9);
	fontTick.setFamily("Microsoft YaHei");
	fontTick.setPointSize(7);
	QColor colorLabel(30, 30, 30);
	QColor colorTick(30, 30, 30);
	ui.qCustomPlot->xAxis->setLabelFont(fontLabel);
	ui.qCustomPlot->xAxis->setLabelColor(colorLabel);
	ui.qCustomPlot->xAxis->setTickLabelFont(fontTick);
	ui.qCustomPlot->xAxis->setTickLabelColor(colorTick);
	ui.qCustomPlot->yAxis->setLabelFont(fontLabel);
	ui.qCustomPlot->yAxis->setLabelColor(colorLabel);
	ui.qCustomPlot->yAxis->setTickLabelFont(fontTick);
	ui.qCustomPlot->yAxis->setTickLabelColor(colorTick);

	ui.qCustomPlot->xAxis2->setVisible(true);
	ui.qCustomPlot->xAxis2->setTickLabels(false);
	ui.qCustomPlot->yAxis2->setVisible(true);
	ui.qCustomPlot->yAxis2->setTickLabels(false);

	ui.qCustomPlot->legend->setFillOrder(QCPLayoutGrid::foColumnsFirst);
	ui.qCustomPlot->legend->setBrush(Qt::transparent);
	ui.qCustomPlot->legend->setWrap(8);
	//ui.qCustomPlot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignLeft | Qt::AlignTop);
	ui.qCustomPlot->legend->setVisible(true);

	// 设置拖拽、放大缩小功能
	ui.qCustomPlot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectPlottables);

	m_timer = new QTimer(this);
	m_timer->setInterval(100);
	m_timer->start();
	connect(m_timer, &QTimer::timeout, this, &DrawWidget::updateShow);

	connect(ui.qCustomPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui.qCustomPlot->xAxis2, SLOT(setRange(QCPRange)));
	connect(ui.qCustomPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui.qCustomPlot->yAxis2, SLOT(setRange(QCPRange)));

	connect(ui.pushButton_save, &QPushButton::clicked, this, &DrawWidget::on_pBt_save);
}

DrawWidget::~DrawWidget()
{
}

void DrawWidget::setLabels(const char * xLabel, const char * yLabel) noexcept
{
	//设置坐标轴标签
	ui.qCustomPlot->xAxis->setLabel(QString::fromLocal8Bit(xLabel));
	ui.qCustomPlot->yAxis->setLabel(QString::fromLocal8Bit(yLabel));
}

void DrawWidget::showLines(int lineSize) noexcept
{
	/********************绘制曲线**********************/
	for (int i = 0; i < lineSize; ++i)
	{
		QPen pen;
		pen.setColor(m_colorVector[i]);
		ui.qCustomPlot->addGraph();
		ui.qCustomPlot->graph(i)->setPen(pen);
		ui.qCustomPlot->graph(i)->setName(QString::fromLocal8Bit("点") + QString::number(i));
		//ui.qCustomPlot->graph(i)->setScatterStyle(QCPScatterStyle::ssCircle);
	}
	ui.qCustomPlot->rescaleAxes();
	ui.qCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

void DrawWidget::setData(QVector<double> xData, QVector<QVector<double>> yData) noexcept
{
	/********************绘制曲线**********************/
	if (ui.qCustomPlot->graphCount() != yData.size())
	{
		return;
	}
	for (int i = 0; i < yData.size(); ++i)
	{
		ui.qCustomPlot->graph(i)->setData(xData, yData[i]);
	}
	ui.qCustomPlot->rescaleAxes();
	ui.qCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

void DrawWidget::addData(double xData,QVector<double> yData) noexcept
{
	/********************绘制实时曲线**********************/
	if (ui.qCustomPlot->graphCount() != yData.size())
	{
		return;
	}
	for (int i = 0; i < yData.size(); ++i)
	{
		ui.qCustomPlot->graph(i)->addData(xData, yData[i]);
	}
	ui.qCustomPlot->rescaleAxes();
}

void DrawWidget::setXRange(double position,int count,Qt::AlignmentFlag alignment) noexcept
{
	ui.qCustomPlot->xAxis->setRange(position, count, alignment);
}

void DrawWidget::setYRange(double upper, double lower) noexcept
{
	auto range = ui.qCustomPlot->yAxis->range();
	double temp_up = upper + abs(upper*0.3);
	double temp_low = lower - abs(lower*0.3);
	double up{}, low{};
	if (range.upper > upper&&range.upper < temp_up)
	{
		up = range.upper;
	}
	else
	{
		up = temp_up;
		
	}
	if (up < MINupper&&up >= -0.001)
	{
		up = MINupper;
	}

	if (range.lower < lower&&range.lower > temp_low)
	{
		low = range.lower;
	}
	else
	{
		low = temp_low;
	}
	if (low > MINlower&&low <= 0.001)
	{
		low = MINlower;
	}
	ui.qCustomPlot->yAxis->setRange(low,up);
}

void DrawWidget::saveBmp(const QString& filePath, int width, int height) noexcept
{
	//ui.qCustomPlot->savePng(filePath, width, height);
	ui.qCustomPlot->saveBmp(filePath, width, height,1.0,-1);
}

void DrawWidget::drawHistogram(QVector<double> index, QVector<double> value,double lower,double upper)
{
	ui.qCustomPlot->legend->setVisible(false);
	this->setWindowTitle(QString::fromLocal8Bit("图像直方图"));
	QCPBars* histBar = new QCPBars(ui.qCustomPlot->xAxis, ui.qCustomPlot->yAxis);
	histBar->setData(index, value);
	ui.qCustomPlot->rescaleAxes();
	ui.qCustomPlot->yAxis->setRange(lower, upper);
}

void DrawWidget::updateShow() noexcept
{
	ui.qCustomPlot->replot(QCustomPlot::rpQueuedReplot);
}

void DrawWidget::initColor() noexcept
{
	/**************建立颜色表***************/
	m_colorVector.append(QColor(255, 0, 0));
	m_colorVector.append(QColor(0, 255, 0));
	m_colorVector.append(QColor(0, 0, 255));

	m_colorVector.append(QColor(0, 0, 0));
	m_colorVector.append(QColor(255, 0, 255));
	m_colorVector.append(QColor(255, 215, 0));

	m_colorVector.append(QColor(85, 107, 47));
	m_colorVector.append(QColor(30, 144, 255));
	m_colorVector.append(QColor(0, 255, 255));
	
	int i;
	float ColorInterval;
	int rgb;
	chart_RGB color;
	for (i = 9; i < 100; i++)
	{
		ColorInterval = 1536 / i;
		rgb = (int)ColorInterval;
		int remainder;
		remainder = i % 5;
		if (remainder == 1) {
			color.R = rgb;
			color.G = 0;
			color.B = 0;
		}
		else if (remainder == 2) {
			color.R = 0;
			color.G = rgb;
			color.B = 0;
		}
		else if (remainder == 3) {
			color.R = 0;
			color.G = 0;
			color.B = rgb;
		}
		else if (remainder == 4) {
			color.R = rgb;
			color.G = rgb;
			color.B = 0;
		}
		else {
			color.R = 0;
			color.G = rgb;
			color.B = rgb;
		}
		m_colorVector.append(QColor(color.R, color.G, color.B));
	}
}

void DrawWidget::on_pBt_save() noexcept
{
	
}
