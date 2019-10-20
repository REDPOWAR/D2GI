#pragma once

#include "../ddraw/dd_ddraw.h"

#include "d2gi_common.h"
#include "d2gi_container.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_prim_single_surf.h"


class D2GIDirectDraw : public DDrawProxy, public D2GIBase
{
	D2GIResourceContainer* m_pResourceContainer;
	D2GIPrimaryFlippableSurface* m_pPrimaryFlippableSurf;
	D2GIPrimarySingleSurface* m_pPrimarySingleSurf;
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
	STDMETHOD(RestoreDisplayMode)();
	STDMETHOD(CreatePalette)(DWORD, LPPALETTEENTRY, D3D7::LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*);

	VOID ReleaseResources();
	VOID LoadResources();
	D2GIPrimaryFlippableSurface* GetPrimaryFlippableSurface() { return m_pPrimaryFlippableSurf; }
	D2GIPrimarySingleSurface* GetPrimarySingleSurface() { return m_pPrimarySingleSurf; }
};
