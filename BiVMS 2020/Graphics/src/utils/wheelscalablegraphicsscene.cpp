#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsSceneWheelEvent>
#include "utils/wheelscalablegraphicsscene.h"


WheelScalableGraphicsScene::WheelScalableGraphicsScene(QObject* parent) noexcept
	: QGraphicsScene(parent) {
}

WheelScalableGraphicsScene::~WheelScalableGraphicsScene()
{

}

void WheelScalableGraphicsScene::wheelEvent(QGraphicsSceneWheelEvent* wheelEvent)
{
	const auto delta = wheelEvent->delta();
	double rate = (delta + 1000.0) / 1000.0;

	auto pView = this->views().value(0);
	rate < 0.1 ? rate = 0.1 : 0;

	if (rate < 1) {
		if (m_currentscale > 0.2 && this->views().length() > 0) {
			pView->scale(rate, rate);
			m_currentscale *= rate;
		}
		else
			wheelEvent->ignore();
	}
	else {
		pView->scale(rate, rate);
		m_currentscale *= rate;
	}

	QGraphicsScene::wheelEvent(wheelEvent);
}
