#pragma once

#include "../ddraw/ddraw.h"


class D2GIDirectDraw : public DDrawProxy
{
public:
	D2GIDirectDraw(IDirectDraw7*);
	virtual ~D2GIDirectDraw();

	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj);
};
