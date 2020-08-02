#pragma once

class ShapeClippingEvents
{
public:
	virtual void exec() = 0;
	virtual void undo() = 0;
	virtual void move() = 0;
	virtual void clip() = 0;
};
