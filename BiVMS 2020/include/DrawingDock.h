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
	//��Ҫ���Ƶ�������Ŀ
	inline void show(int linesCount) noexcept
	{
		_close();
		m_drawWidget_X->showLines(linesCount);
		m_drawWidget_Y->showLines(linesCount);
		m_drawWidget_Z->showLines(linesCount);
		m_linesCount = linesCount;
	}
	//���POI���
	void addPOIPlugin() noexcept;
	//ɾ��POI���
	void deletePOIPlugin(const int& index) noexcept;
	//���POI���
	void clearPOIPlugin() noexcept;
	//����POI�����ʾֵ
	void setPOIValue(const int & index, const double & xValue, const double & yValue, const double & zValue);
	//�������߼������ʾ����
	void setData(QVector<double> time, QVector<double> xData, QVector<double> yData, QVector<double> zData) noexcept;
	//������ʾ
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
	QVector<POIPlugin*>                  m_POIPlugins;              //��������POI���
	POIPlugin*                           m_POIPlugin_first = nullptr;//���ͷ
	DrawWidget*                          m_drawWidget_X = nullptr;  //x����
	DrawWidget*                          m_drawWidget_Y = nullptr;  //y����
	DrawWidget*                          m_drawWidget_Z = nullptr;  //z����
	
	int                                  m_number{};                //��ʾ��������
	QVector<double>                      m_data_Time;               //ʱ���������
	QVector<double>                      m_data_X;                  //x����λ��
	QVector<double>                      m_data_Y;                  //y����λ��
	QVector<double>                      m_data_Z;                  //z����λ��
	int                                  m_linesCount{};
	QTimer*                              m_timer = nullptr;         //��ʱ��-������ʾ

};
