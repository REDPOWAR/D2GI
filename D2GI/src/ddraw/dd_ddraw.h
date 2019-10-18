#pragma once

#include <ddraw.h>

#include "../common.h"


class DDrawProxy : public IDirectDraw7, public Unknown
{
public:
	DDrawProxy();
	virtual ~DDrawProxy();

	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) { return Unknown::QueryInterface(riid, ppvObj); };
	STDMETHOD_(ULONG, AddRef) () { return Unknown::AddRef(); };
	STDMETHOD_(ULONG, Release) () { return Unknown::Release(); };

	STDMETHOD(Compact)();
	STDMETHOD(CreateClipper)(DWORD, LPDIRECTDRAWCLIPPER FAR*, IUnknown FAR*) ;
	STDMETHOD(CreatePalette)( DWORD, LPPALETTEENTRY, LPDIRECTDRAWPALETTE FAR*, IUnknown FAR*) ;
	STDMETHOD(CreateSurface)(  LPDDSURFACEDESC2, LPDIRECTDRAWSURFACE7 FAR*, IUnknown FAR*) ;
	STDMETHOD(DuplicateSurface)( LPDIRECTDRAWSURFACE7, LPDIRECTDRAWSURFACE7 FAR*) ;
	STDMETHOD(EnumDisplayModes)( DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMMODESCALLBACK2) ;
	STDMETHOD(EnumSurfaces)( DWORD, LPDDSURFACEDESC2, LPVOID, LPDDENUMSURFACESCALLBACK7) ;
	STDMETHOD(FlipToGDISurface)() ;
	STDMETHOD(GetCaps)( LPDDCAPS, LPDDCAPS) ;
	STDMETHOD(GetDisplayMode)( LPDDSURFACEDESC2) ;
	STDMETHOD(GetFourCCCodes)(  LPDWORD, LPDWORD) ;
	STDMETHOD(GetGDISurface)( LPDIRECTDRAWSURFACE7 FAR*) ;
	STDMETHOD(GetMonitorFrequency)( LPDWORD) ;
	STDMETHOD(GetScanLine)( LPDWORD) ;
	STDMETHOD(GetVerticalBlankStatus)( LPBOOL) ;
	STDMETHOD(Initialize)( GUID FAR*) ;
	STDMETHOD(RestoreDisplayMode)() ;
	STDMETHOD(SetCooperativeLevel)( HWND, DWORD) ;
	STDMETHOD(SetDisplayMode)( DWORD, DWORD, DWORD, DWORD, DWORD) ;
	STDMETHOD(WaitForVerticalBlank)( DWORD, HANDLE) ;
	STDMETHOD(GetAvailableVidMem)( LPDDSCAPS2, LPDWORD, LPDWORD) ;
	STDMETHOD(GetSurfaceFromDC) ( HDC, LPDIRECTDRAWSURFACE7*) ;
	STDMETHOD(RestoreAllSurfaces)() ;
	STDMETHOD(TestCooperativeLevel)() ;
	STDMETHOD(GetDeviceIdentifier)( LPDDDEVICEIDENTIFIER2, DWORD) ;
	STDMETHOD(StartModeTest)( LPSIZE, DWORD, DWORD) ;
	STDMETHOD(EvaluateMode)( DWORD, DWORD*) ;
};