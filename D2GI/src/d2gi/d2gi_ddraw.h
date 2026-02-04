#pragma once

#include "../ddraw/dd_ddraw.h"
#include "../d3d/d3d_direct3d.h"

#include "d2gi_common.h"
#include "d2gi_container.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_prim_single_surf.h"


class D2GIDirectDraw : public DDrawProxy, public D3DProxy, public D2GIBase, public Unknown
{
	D2GIResourceContainer* m_pResourceContainer;
	D2GIPrimaryFlippableSurface* m_pPrimaryFlippableSurf;
	D2GIPrimarySingleSurface* m_pPrimarySingleSurf;
public:
	D2GIDirectDraw(D2GI*);
	virtual ~D2GIDirectDraw();

	STDMETHOD_(ULONG, AddRef) () { return Unknown::AddRef(); };
	STDMETHOD_(ULONG, Release) () { return Unknown::Release(); };
	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj);

	// DDrawProxy methods
	STDMETHOD(CreateSurface)(D3D7::LPDDSURFACEDESC2, D3D7::LPDIRECTDRAWSURFACE7 FAR*, IUnknown FAR*);
	STDMETHOD(SetCooperativeLevel)(HWND, DWORD);
	STDMETHOD(SetDisplayMode)(DWORD, DWORD, DWORD, DWORD, DWORD);
	STDMETHOD(EnumDisplayModes)(DWORD, D3D7::LPDDSURFACEDESC2, LPVOID, D3D7::LPDDENUMMODESCALLBACK2);
	STDMETHOD(GetCaps)(D3D7::LPDDCAPS, D3D7::LPDDCAPS);
	STDMETHOD(GetDeviceIdentifier)(D3D7::LPDDDEVICEIDENTIFIER2, DWORD);
	STDMETHOD(GetAvailableVidMem)(D3D7::LPDDSCAPS2, LPDWORD, LPDWORD);
	STDMETHOD(RestoreDisplayMode)();
	STDMETHOD(CreatePalette)(DWORD, LPPALETTEENTRY, D3D7::LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*);

	// D3DProxy methods
	STDMETHOD(CreateDevice)(REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*) override;
	STDMETHOD(EnumDevices)(D3D7::LPD3DENUMDEVICESCALLBACK7, LPVOID) override;
	STDMETHOD(EnumZBufferFormats)(REFCLSID, D3D7::LPD3DENUMPIXELFORMATSCALLBACK, LPVOID) override;

	VOID ReleaseResources();
	VOID LoadResources();
	D2GIPrimaryFlippableSurface* GetPrimaryFlippableSurface() { return m_pPrimaryFlippableSurf; }
	D2GIPrimarySingleSurface* GetPrimarySingleSurface() { return m_pPrimarySingleSurf; }
};
