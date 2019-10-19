#pragma once

#include "../common.h"

#include "../d3d7.h"


class DDrawProxy : public D3D7::IDirectDraw7, public Unknown
{
public:
	DDrawProxy();
	virtual ~DDrawProxy();

	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) { return Unknown::QueryInterface(riid, ppvObj); };
	STDMETHOD_(ULONG, AddRef) () { return Unknown::AddRef(); };
	STDMETHOD_(ULONG, Release) () { return Unknown::Release(); };

	STDMETHOD(Compact)();
	STDMETHOD(CreateClipper)(DWORD, D3D7::LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR*) ;
	STDMETHOD(CreatePalette)( DWORD, LPPALETTEENTRY, D3D7::LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*) ;
	STDMETHOD(CreateSurface)(D3D7::LPDDSURFACEDESC2, D3D7::LPDIRECTDRAWSURFACE7 FAR*, IUnknown FAR*) ;
	STDMETHOD(DuplicateSurface)(D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECTDRAWSURFACE7 FAR*) ;
	STDMETHOD(EnumDisplayModes)( DWORD, D3D7::LPDDSURFACEDESC2, LPVOID, D3D7::LPDDENUMMODESCALLBACK2) ;
	STDMETHOD(EnumSurfaces)( DWORD, D3D7::LPDDSURFACEDESC2, LPVOID, D3D7::LPDDENUMSURFACESCALLBACK7) ;
	STDMETHOD(FlipToGDISurface)() ;
	STDMETHOD(GetCaps)(D3D7::LPDDCAPS, D3D7::LPDDCAPS) ;
	STDMETHOD(GetDisplayMode)(D3D7::LPDDSURFACEDESC2) ;
	STDMETHOD(GetFourCCCodes)(  LPDWORD, LPDWORD) ;
	STDMETHOD(GetGDISurface)(D3D7::LPDIRECTDRAWSURFACE7 FAR*) ;
	STDMETHOD(GetMonitorFrequency)( LPDWORD) ;
	STDMETHOD(GetScanLine)( LPDWORD) ;
	STDMETHOD(GetVerticalBlankStatus)( LPBOOL) ;
	STDMETHOD(Initialize)( GUID FAR*) ;
	STDMETHOD(RestoreDisplayMode)() ;
	STDMETHOD(SetCooperativeLevel)( HWND, DWORD) ;
	STDMETHOD(SetDisplayMode)( DWORD, DWORD, DWORD, DWORD, DWORD) ;
	STDMETHOD(WaitForVerticalBlank)( DWORD, HANDLE) ;
	STDMETHOD(GetAvailableVidMem)(D3D7::LPDDSCAPS2, LPDWORD, LPDWORD) ;
	STDMETHOD(GetSurfaceFromDC) ( HDC, D3D7::LPDIRECTDRAWSURFACE7*) ;
	STDMETHOD(RestoreAllSurfaces)() ;
	STDMETHOD(TestCooperativeLevel)() ;
	STDMETHOD(GetDeviceIdentifier)(D3D7::LPDDDEVICEIDENTIFIER2, DWORD) ;
	STDMETHOD(StartModeTest)( LPSIZE, DWORD, DWORD) ;
	STDMETHOD(EvaluateMode)( DWORD, DWORD*) ;
};
