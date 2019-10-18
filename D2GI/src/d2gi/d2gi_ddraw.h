#pragma once

#include "d2gi_common.h"
#include "../ddraw/dd_ddraw.h"


class D2GIDirectDraw : public DDrawProxy, public D2GIBase
{
public:
	D2GIDirectDraw(D2GI*);
	virtual ~D2GIDirectDraw();

	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj);
	STDMETHOD(CreateSurface)(LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7 FAR*, IUnknown FAR*);
	STDMETHOD(SetCooperativeLevel)(HWND, DWORD);
	STDMETHOD(SetDisplayMode)(DWORD, DWORD, DWORD, DWORD, DWORD);
};
