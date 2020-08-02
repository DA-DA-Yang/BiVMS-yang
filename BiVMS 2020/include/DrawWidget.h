/***************************************************************************
**  BiVMS 2020                                                            **
**  A widget that displays data curve                                     **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once
#include <QWidget>
#include "ui_DrawWidget.h"

#define MINupper 0.1
#define MINlower -0.1
struct chart_RGB {
	int R;
	int G;
	int B;
};

class DrawWidget : public QWidget
{
	Q_OBJECT

public:
	DrawWidget( QWidget *parent = Q_NULLPTR);
	~DrawWidget();

	/*利用ui进行绘图设置*/
	inline Ui::DrawWidget UI() { return ui; }
	void setLabels(const char * xLabel, const char * yLabel) noexcept;               
	void showLines(int lineSize) noexcept;
	void close() noexcept {
		ui.qCustomPlot->clearGraphs(); 
		ui.qCustomPlot->clearPlottables();
		ui.qCustomPlot->replot();
	}
	void setData(QVector<double> xData, QVector<QVector<double>> yData) noexcept;
	void addData(double xData, QVector<double> yData) noexcept;
	void setXRange(double position, int count, Qt::AlignmentFlag alignment) noexcept;
	void setYRange(double upper, double lower) noexcept;
	void saveBmp(const QString& filePath,int width,int height) noexcept;
	void drawHistogram(QVector<double> index, QVector<double> value, double lower, double upper);
	void updateShow() noexcept;
	void initColor() noexcept;

private slots:
	void on_pBt_save() noexcept;


private:
	Ui::DrawWidget ui;
	QVector<QColor>                 m_colorVector;
	QTimer*                         m_timer = nullptr;
};
