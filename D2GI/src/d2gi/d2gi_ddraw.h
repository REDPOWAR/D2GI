#pragma once

#include "../ddraw/dd_ddraw.h"
#include "../d3d/d3d_direct3d.h"

#include "d2gi_common.h"
#include "d2gi_container.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_prim_single_surf.h"

#include "../common/implements_proxy.hpp"


class D2GIDirectDraw : public ImplementsProxy<DDrawProxy, D3DProxy>, public D2GIBase
{
private:
	D2GIResourceContainer m_resourceContainer;
	D2GIPrimaryFlippableSurface* m_pPrimaryFlippableSurf = nullptr;
	D2GIPrimarySingleSurface* m_pPrimarySingleSurf = nullptr;
public:
	D2GIDirectDraw(D2GI*);
	virtual ~D2GIDirectDraw() override;

	// DDrawProxy methods
	STDMETHOD(CreateSurface)(D3D7::LPDDSURFACEDESC2, D3D7::LPDIRECTDRAWSURFACE7 FAR*, IUnknown FAR*) override;
	STDMETHOD(SetCooperativeLevel)(HWND, DWORD) override;
	STDMETHOD(SetDisplayMode)(DWORD, DWORD, DWORD, DWORD, DWORD) override;
	STDMETHOD(EnumDisplayModes)(DWORD, D3D7::LPDDSURFACEDESC2, LPVOID, D3D7::LPDDENUMMODESCALLBACK2) override;
	STDMETHOD(GetCaps)(D3D7::LPDDCAPS, D3D7::LPDDCAPS) override;
	STDMETHOD(GetDeviceIdentifier)(D3D7::LPDDDEVICEIDENTIFIER2, DWORD) override;
	STDMETHOD(GetAvailableVidMem)(D3D7::LPDDSCAPS2, LPDWORD, LPDWORD) override;
	STDMETHOD(RestoreDisplayMode)() override;
	STDMETHOD(CreatePalette)(DWORD, LPPALETTEENTRY, D3D7::LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*) override;

	// D3DProxy methods
	STDMETHOD(CreateDevice)(REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*) override;
	STDMETHOD(EnumDevices)(D3D7::LPD3DENUMDEVICESCALLBACK7, LPVOID) override;
	STDMETHOD(EnumZBufferFormats)(REFCLSID, D3D7::LPD3DENUMPIXELFORMATSCALLBACK, LPVOID) override;

	VOID ReleaseResources();
	VOID LoadResources();
	D2GIPrimaryFlippableSurface* GetPrimaryFlippableSurface() const { return m_pPrimaryFlippableSurf; }
	D2GIPrimarySingleSurface* GetPrimarySingleSurface() const { return m_pPrimarySingleSurf; }
};
