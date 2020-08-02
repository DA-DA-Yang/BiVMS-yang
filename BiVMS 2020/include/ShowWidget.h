/***************************************************************************
**  BiVMS 2020                                                            **
**  A widget that displays image and define POIs		                  **

****************************************************************************
**           Author: DA YANG                                              **
**          Contact: yangda@shu.edu.cn                                    **
**             Date: 26.07.2020                                           **
**          Version: 1.0                                                  **
****************************************************************************/
#pragma once

#include <QWidget>
#include "ui_ShowWidget.h"
#include <utils/editablegraphicsscene.h>

class ShowWidget : public QWidget
{
	Q_OBJECT

public:
	ShowWidget(QWidget *parent = Q_NULLPTR);
	~ShowWidget();

	void showImage(QImage& img) noexcept;
	void showFPS(int FPS) noexcept;
	inline const EditableGraphicsScene* scene() noexcept { return m_scene; }

	inline void add_Point() noexcept 
	{ 
		m_scene->draw(GraphicEditorStatus::DRAW_POINT); 
	}
	inline void delete_Point() noexcept 
	{ 
		m_scene->draw(GraphicEditorStatus::DEL_NODE); 
	}
	inline void clear_Point() noexcept 
	{ 
		m_scene->ClearALLBGPoints();
		m_scene->ClearALLStartPoints();
		m_scene->draw(GraphicEditorStatus::READY);
		emit signal_ClearPoint();
	}

private slots:
	void _send_Selected(int index) noexcept;   //发送测点被选择信号

signals:
	void signal_AddPoint();          //已添加一个测点,index为测点ID
	void signal_DeletePoint(int index);       //已删除一个测点,index为测点ID
	void signal_ClearPoint();                 //已删除所有测点
private:
	Ui::ShowWidget ui;

	EditableGraphicsScene*		m_scene = nullptr;
	QGraphicsPixmapItem*        m_displayItem = nullptr;
};
