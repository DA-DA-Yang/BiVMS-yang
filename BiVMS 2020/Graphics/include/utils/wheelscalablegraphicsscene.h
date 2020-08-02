#pragma once

#include<QGraphicsScene>

class QGraphicsSceneWheelEvent;

class WheelScalableGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	using value_type = double;
	WheelScalableGraphicsScene(QObject* parent = nullptr) noexcept;
	~WheelScalableGraphicsScene();

	inline const value_type& scale() const noexcept {
		return m_currentscale;
	}
	inline value_type& scale() noexcept {
		return m_currentscale;
	}
private:
	void wheelEvent(QGraphicsSceneWheelEvent*e);

public:
	value_type m_currentscale = 1;
};