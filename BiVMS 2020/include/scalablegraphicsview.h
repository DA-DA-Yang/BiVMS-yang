#pragma once
#include "ui_scalablegraphicsview.h"
#include <QGraphicsView>
#include <QSpacerItem>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWheelEvent>
#include <QFocusEvent>
#include <QSettings>
#include <QLabel>
#include <QSlider>

class ScalableGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	static constexpr auto max_zoom_value = 80;
	static constexpr auto default_zoom_value = 15;
	ScalableGraphicsView(QWidget* parent = Q_NULLPTR);
	~ScalableGraphicsView();

	/* methods */
public:
	void show_tip(const QString& _tip);
	void show_imname(const QString& _imgname);
	void show_mean(const QString& _mean);
	void set_ctrls_visible(bool _flag);
	void adaptive_ctrl(int _w, int _h);
	void focus_on_subset(const QRect& _rect);

	/* slots */
public slots:
	void slot_autofit();
	void zoom_changed(int value)noexcept {
		_Zoom_changed(value);
	}
	void change_cursor_shape(Qt::CursorShape shape)noexcept {
		this->viewport()->setCursor(shape);
	}
private slots:
	void _Zoom_changed(int _val);
	void _Zoom_in();
	void _Zoom_out();
	void slotMenuZoomIn();
	void slotMenuZoomOut();
	void slotMenuResume();

	/* signals */
signals:
	void signal_zoom_changed();
	void signal_double_changed();
	void signal_focus_on();

	/* events */
private:
	//void mouseMoveEvent(QMouseEvent* event);
	void mouseDoubleClickEvent(QMouseEvent* event);
	void wheelEvent(QWheelEvent* event);
	void keyPressEvent(QKeyEvent* event);
	void focusOnEvent(QFocusEvent* event);
	void resizeEvent(QResizeEvent* event);

protected:
	void changeEvent(QEvent* e);

	/* fields */
public:
	QPushButton* pbtnFocusSubset = nullptr, * pbtnFocusROI = nullptr;
	QPushButton* pbtnAZoom = nullptr, * pbtnZoomIn = nullptr,
		* pbtnZoomOut = nullptr;
	QLabel* plbTip = nullptr, * plbMean = nullptr, * plbImgName = nullptr,
		* plb_1 = nullptr, * plb_2 = nullptr;
	QSlider* m_zoomslider = nullptr;

private:
	Ui::ScalableGraphicsView ui;
	int m_slidervalue;

};

