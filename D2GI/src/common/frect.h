#pragma once

#include "common.h"


struct FRECT
{
	FLOAT fLeft, fTop, fRight, fBottom;

	FRECT(){}
	FRECT(FLOAT l, FLOAT t, FLOAT r, FLOAT b)
		: fLeft(l), fTop(t), fRight(r), fBottom(b)
	{
	}
	FRECT(const RECT& rt)
		: fLeft((FLOAT)rt.left), fTop((FLOAT)rt.top), fRight((FLOAT)rt.right), fBottom((FLOAT)rt.bottom)
	{
	}

	FLOAT GetWidth() const { return fRight - fLeft; };
	FLOAT GetHeight() const { return fBottom - fTop; };
	FLOAT GetXCenter() const { return 0.5f * (fLeft + fRight);  }
	FLOAT GetYCenter() const { return 0.5f * (fTop + fBottom); }
};
