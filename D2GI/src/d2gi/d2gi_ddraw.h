#pragma once

#include "../ddraw/dd_ddraw.h"

#include "d2gi_common.h"
#include "d2gi_container.h"


class D2GIDirectDraw : public DDrawProxy, public D2GIBase
{
	D2GIResourceContainer* m_pSurfaceContainer;
public:
	D2GIDirectDraw(D2GI*);
	virtual ~D2GIDirectDraw();

	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj);
	STDMETHOD(CreateSurface)(D3D7::LPDDSURFACEDESC2, D3D7::LPDIRECTDRAWSURFACE7 FAR*, IUnknown FAR*);
	STDMETHOD(SetCooperativeLevel)(HWND, DWORD);
	STDMETHOD(SetDisplayMode)(DWORD, DWORD, DWORD, DWORD, DWORD);
	STDMETHOD(EnumDisplayModes)(DWORD, D3D7::LPDDSURFACEDESC2, LPVOID, D3D7::LPDDENUMMODESCALLBACK2);
	STDMETHOD(GetCaps)(D3D7::LPDDCAPS, D3D7::LPDDCAPS);
	STDMETHOD(GetDeviceIdentifier)(D3D7::LPDDDEVICEIDENTIFIER2, DWORD);
	STDMETHOD(GetAvailableVidMem)(D3D7::LPDDSCAPS2, LPDWORD, LPDWORD);

	VOID ReleaseResources();
	VOID LoadResources();
};
