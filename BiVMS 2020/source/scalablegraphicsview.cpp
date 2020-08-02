#include "scalablegraphicsview.h"
#include "Utils/macros.hpp"
#include "Utils/forward.hpp"
#include <qmath.h>

ScalableGraphicsView::ScalableGraphicsView(QWidget *parent)
	: QGraphicsView(parent), m_zoomslider(new QSlider()),
	plbTip(new QLabel()), plb_1(new QLabel()), plb_2(new QLabel()),
	plbImgName(new QLabel()), plbMean(new QLabel()),
	pbtnAZoom(new QPushButton), pbtnFocusROI(new QPushButton),
	pbtnFocusSubset(new QPushButton), pbtnZoomIn(new QPushButton),
	pbtnZoomOut(new QPushButton)
{
	ui.setupUi(this);
	setStyleSheet("QLabel {background : rgba(224,224,224, 127);}");

	// initialization of controls
	plbTip->setVisible(false);
	plbMean->setVisible(false);
	plbImgName->setVisible(false);
	pbtnFocusSubset->setVisible(false);
	pbtnFocusROI->setVisible(false);
	pbtnAZoom->setFixedSize(20, 20);
	pbtnAZoom->setProperty("ScalableGraphicsView", true);
	pbtnZoomIn->setFixedSize(20, 20);
	pbtnZoomIn->setProperty("ScalableGraphicsView", true);
	pbtnZoomOut->setFixedSize(20, 20);
	pbtnZoomOut->setProperty("ScalableGraphicsView", true);
	pbtnFocusSubset->setFixedSize(20, 20);
	pbtnFocusSubset->setProperty("ScalableGraphicsView", true);
	pbtnFocusROI->setFixedSize(20, 20);
	pbtnFocusROI->setProperty("ScalableGraphicsView", true);

	m_zoomslider->setOrientation(Qt::Horizontal);
	m_zoomslider->setRange(0, max_zoom_value);
	m_zoomslider->setValue(default_zoom_value);
	m_zoomslider->setPageStep(1);
	m_zoomslider->setFixedSize(100, 16);
	m_zoomslider->setFocusPolicy(Qt::NoFocus);

	plb_1->setFixedSize(100, 2);
	plb_2->setFixedSize(100, 2);
	plbImgName->setAlignment(Qt::AlignCenter);

	// layout
	QVBoxLayout* pVBLayout = new QVBoxLayout();
	//pVBLayout->addWidget(plb_1);
	pVBLayout->addWidget(m_zoomslider);
	//pVBLayout->addWidget(plb_2);
	QSpacerItem* pHSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	QHBoxLayout* pHBLayout = new QHBoxLayout();
	/*pHBLayout->addWidget(plbTip);
	pHBLayout->addWidget(pbtnZoomIn);
	pHBLayout->addLayout(pVBLayout);
	pHBLayout->addWidget(pbtnZoomOut);*/
	pHBLayout->addSpacerItem(pHSpacer);
	pHBLayout->addWidget(pbtnAZoom);
	pbtnAZoom->setStyleSheet("background-color:rgba(240,240,240,0)");
	pbtnAZoom->setIcon(QPixmap(
		"E:/Projects/phoMech/phoMech/Resources/icons/zoomfit.png"));
	/*pHBLayout->addWidget(pbtnFocusROI);
	pHBLayout->addWidget(pbtnFocusSubset);
	pHBLayout->addWidget(plbMean);*/
	pHBLayout->setSpacing(0);
	QSpacerItem* pVSpacer = new QSpacerItem(20, 40, 
		QSizePolicy::Minimum, QSizePolicy::Expanding);
	QVBoxLayout* pVLayout = new QVBoxLayout(this);
	pVLayout->addLayout(pHBLayout);
	pVLayout->addSpacerItem(pVSpacer);
	pVLayout->addWidget(plbImgName);
	pVLayout->setContentsMargins(0, 0, 0, 0);

	m_slidervalue = m_zoomslider->value();
	this->viewport()->setMouseTracking(true);
	// connection of signals and slots 
	CONNECT_TO_THIS(pbtnAZoom, clicked, slot_autofit);
	CONNECT_TO_THIS(pbtnZoomIn, clicked, _Zoom_in);
	CONNECT_TO_THIS(pbtnZoomOut, clicked, _Zoom_out);
	CONNECT_TO_THIS(m_zoomslider, valueChanged, _Zoom_changed);
}

ScalableGraphicsView::~ScalableGraphicsView() {}

#pragma region Public Methods
void ScalGraphicsView::show_tip(const QString &_tip)
{
	plbTip->setVisible(true);
	plbTip->setText(_tip);
}

void ScalGraphicsView::show_mean(const QString &mean)
{
	plbMean->setVisible(true);
	plbMean->setText(mean);
}

void ScalGraphicsView::show_imname(const QString &name)
{
	plbImgName->setVisible(true);
	plbImgName->setText(name);
}

void ScalGraphicsView::set_ctrls_visible(bool is_visible)
{
	pbtnAZoom->setVisible(is_visible);
	pbtnZoomIn->setVisible(is_visible);
	pbtnZoomOut->setVisible(is_visible);
	m_zoomslider->setVisible(is_visible);
	plb_1->setVisible(is_visible);
	plb_2->setVisible(is_visible);
}

void ScalGraphicsView::focus_on_subset(const QRect &rect)
{
	QTransform _Transf = this->transform();
	double _PreScaleFactor = _Transf.m11();

	this->fitInView(rect, Qt::KeepAspectRatio);

	const auto _CurScaleFactor = this->transform().m11();
	const auto involution = qLn(_CurScaleFactor / _PreScaleFactor)
		/ qLn(1.25);

	int count = involution;
	count = (involution - count)>0.5 ? count++ : count;
	count = (count - involution)>0.5 ? count-- : count;

	this->setTransform(_Transf);

	count += m_slidervalue;
	count < default_zoom_value ? count = default_zoom_value : count;
	m_zoomslider->setValue(count);
}
/* adaptive display according user input parameters if image resolution is low */
void ScalGraphicsView::adaptive_ctrl(int _w, int _h)
{
	setSceneRect(0, 0, _w, _h);
	slot_autofit();
}
#pragma endregion

#pragma region Slots
void ScalGraphicsView::_Zoom_changed(int _val)
{
	pbtnZoomIn->setEnabled(true);
	pbtnZoomOut->setEnabled(true);

	if (_val == default_zoom_value){
		this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
		m_zoomslider->setValue(default_zoom_value);
	}
	else{
		if (_val > m_slidervalue){
			for (int i = 0; i<_val - m_slidervalue; i++)
				this->scale(1.15, 1.15);
			if (_val == max_zoom_value)
				pbtnZoomOut->setEnabled(false);
		}
		else if (_val < m_slidervalue){
			for (int i = 0; i< m_slidervalue - _val; i++)
				this->scale(0.85, 0.85);
			if (_val == 0)
				pbtnZoomIn->setEnabled(false);
		}
	}
	m_slidervalue = _val;

	emit signal_zoom_changed();
}

//fit in button, set the value of slider to 15
void ScalGraphicsView::slot_autofit()
{
	_Zoom_changed(default_zoom_value);
}

//zoom in by decreasing the value of slider by 1 
void ScalGraphicsView::_Zoom_in()
{
	m_zoomslider->setValue(m_slidervalue - 1);
}

//zoom out by increasing the value of slider by 1 
void ScalGraphicsView::_Zoom_out()
{
	m_zoomslider->setValue(m_slidervalue + 1);
}

void ScalGraphicsView::slotMenuZoomIn()
{
	if (hasFocus()) _Zoom_out();
}

void ScalGraphicsView::slotMenuZoomOut()
{
	if (hasFocus()) _Zoom_in();
}


void ScalGraphicsView::slotMenuResume()
{
	if (hasFocus()) slot_autofit();
}

#pragma endregion

#pragma region Events
void ScalGraphicsView::resizeEvent(QResizeEvent *resizeEvent)
{
	this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);

	m_zoomslider->setValue(default_zoom_value);

	QGraphicsView::resizeEvent(resizeEvent);
}

//Plus 1 if wheel forward, else minus 1 
void ScalGraphicsView::wheelEvent(QWheelEvent *event)
{
	event->delta() > 0 ? _Zoom_out() : _Zoom_in();
}

void ScalGraphicsView::mouseDoubleClickEvent(QMouseEvent* event)
{
	emit signal_double_changed();
}
//void ScalableGraphicsView::mouseMoveEvent(QMouseEvent * event)
//{
//	const auto _Srect = this->mapToScene(rect()).boundingRect();
//	auto _Rect = this->sceneRect();
//	/*_Rect.setLeft(abs(_Srect.left()));
//	_Rect.setTop(abs(_Srect.top()));*/
//	if (const auto pos = event->pos(); _Rect.contains(pos)) {
//		this->viewport()->setCursor(Qt::CrossCursor);
//	}
//	else {
//		this->viewport()->setCursor(Qt::PointingHandCursor);
//	}
//}
void ScalGraphicsView::keyPressEvent(QKeyEvent * event)
{
	switch (event->key()) {
	case Qt::Key_Equal:      // ctrl+  zoom in
		if (event->modifiers() & Qt::ControlModifier)
			m_zoomslider->setValue(m_slidervalue + 1);
		break;
	case Qt::Key_Minus:		// ctrl-  zoom out
		if (event->modifiers() & Qt::ControlModifier)
			m_zoomslider->setValue(m_slidervalue - 1);
		break;
	case Qt::Key_0:         // ctrl0  resume
		if (event->modifiers() & Qt::ControlModifier)
			slot_autofit();
	default:
		QWidget::keyPressEvent(event);
	}
}

void ScalGraphicsView::focusOnEvent(QFocusEvent* event)
{
	emit signal_focus_on();
}

void ScalGraphicsView::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
}
#pragma endregion