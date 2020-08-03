/***************************************************************************
**  BiVMS 2020                                                            **
**  In MainWnd                                                            **
**  A dockWidget that displays realtime data curve                        **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once
#include <QDockWidget>
#include <QVector>
#include "ui_DrawingDock.h"
#include <QTableWidget>
#include "DrawWidget.h"
#include "POIPlugin.h"
class DrawingDock : public QDockWidget
{
	Q_OBJECT

public:
	DrawingDock(QWidget *parent = Q_NULLPTR);
	~DrawingDock();

	inline Ui::DrawingDock UI() noexcept { return ui; }
	//需要绘制的曲线数目
	inline void show(int linesCount) noexcept
	{
		_close();
		m_drawWidget_X->showLines(linesCount);
		m_drawWidget_Y->showLines(linesCount);
		m_drawWidget_Z->showLines(linesCount);
		m_linesCount = linesCount;
	}
	//添加POI插件
	void addPOIPlugin() noexcept;
	//删除POI插件
	void deletePOIPlugin(const int& index) noexcept;
	//清除POI插件
	void clearPOIPlugin() noexcept;
	//设置POI插件显示值
	void setPOIValue(const int & index, const double & xValue, const double & yValue, const double & zValue);
	//设置曲线及插件显示数据
	void setData(QVector<double> time, QVector<double> xData, QVector<double> yData, QVector<double> zData) noexcept;
	//设置显示
	void setXRange();
	void setYRange();
private:
	inline void _close() noexcept
	{
		_clearData();
		m_drawWidget_X->close();
		m_drawWidget_Y->close();
		m_drawWidget_Z->close();
	}
	inline void _clearData() noexcept
	{
		m_data_Time.clear();
		m_data_X.clear();
		m_data_Y.clear();
		m_data_Z.clear();
	}
private slots:
	void _updateShow() noexcept;
	void _selected_AllPOIPlugins(bool checked) noexcept;
	void on_checkBox_showX() noexcept;
	void on_checkBox_showY() noexcept;
	void on_checkBox_showZ() noexcept;
	void on_pBt_setting() noexcept;
	
private:
	Ui::DrawingDock                      ui;
	QVector<POIPlugin*>                  m_POIPlugins;              //包含所有POI插件
	POIPlugin*                           m_POIPlugin_first = nullptr;//插件头
	DrawWidget*                          m_drawWidget_X = nullptr;  //x曲线
	DrawWidget*                          m_drawWidget_Y = nullptr;  //y曲线
	DrawWidget*                          m_drawWidget_Z = nullptr;  //z曲线
	
	int                                  m_number{};                //显示数据数量
	QVector<double>                      m_data_Time;               //时间横轴数据
	QVector<double>                      m_data_X;                  //x方向位移
	QVector<double>                      m_data_Y;                  //y方向位移
	QVector<double>                      m_data_Z;                  //z方向位移
	int                                  m_linesCount{};
	QTimer*                              m_timer = nullptr;         //定时器-控制显示

};
