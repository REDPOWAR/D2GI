#pragma once

#include "common.h"


struct FRECT
{
	FLOAT fLeft, fTop, fRight, fBottom;

	FRECT(){}
	FRECT(FLOAT l, FLOAT t, FLOAT r, FLOAT b)
	{
		fLeft = l; fTop = t; fRight = r; fBottom = b;
	}
	FRECT(CONST RECT& rt)
	{
		fLeft = (FLOAT)rt.left; fTop = (FLOAT)rt.top; fRight = (FLOAT)rt.right; fBottom = (FLOAT)rt.bottom;
	}

	FLOAT GetWidth() { return fRight - fLeft; };
	FLOAT GetHeight() { return fBottom - fTop; };
	FLOAT GetXCenter() { return 0.5f * (fLeft + fRight);  }
	FLOAT GetYCenter() { return 0.5f * (fTop + fBottom); }
};
