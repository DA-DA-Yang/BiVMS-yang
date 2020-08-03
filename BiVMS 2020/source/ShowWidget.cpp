#include "ShowWidget.h"

ShowWidget::ShowWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	m_scene = new EditableGraphicsScene(this);
	ui.graphicsView->setScene(m_scene);
	m_scene->draw(GraphicEditorStatus::READY);
	connect(m_scene, &EditableGraphicsScene::selectedIndex, this, &ShowWidget::_send_Selected);

	//更换鼠标类型
	connect(m_scene, &EditableGraphicsScene::change_cursor_shape, ui.graphicsView, &ScalableGraphicsView::change_cursor_shape);
}

ShowWidget::~ShowWidget()
{
}

void ShowWidget::showImage(QImage& img) noexcept
{
	//显示图像
	if (m_displayItem)
	{
		m_scene->removeItem(m_displayItem);
		delete m_displayItem;
		m_displayItem = nullptr;
	}
	if (img.bits())
	{
		QPixmap pixmap = QPixmap::fromImage(img);
		m_displayItem = m_scene->addPixmap(pixmap);
		m_scene->m_showImageItem = m_displayItem;
	}
}

void ShowWidget::showFPS(int FPS) noexcept
{
	QString text = "FPS: " + QString::number(FPS);
	ui.label_FPS->setText(text);
}

void ShowWidget::showTime(QString strTime) noexcept
{
	QString text = "Time: " + strTime;
	ui.label_FPS->setText(text);
}

void ShowWidget::_send_Selected(int index) noexcept
{
	//发送测点被选择信号
	if (m_scene->draw_status() == GraphicEditorStatus::DRAW_POINT)
	{
		emit signal_AddPoint();
	}
	if (m_scene->draw_status() == GraphicEditorStatus::DEL_NODE)
	{
		emit signal_DeletePoint(index);
	}
	
}
