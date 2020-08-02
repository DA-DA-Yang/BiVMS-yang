#pragma once

class ShapeEvent
{
public:
	virtual void exec() = 0;
	virtual void move() = 0;
	virtual void merge() = 0;
	virtual void undo() = 0;
	virtual void redo() = 0;
};
