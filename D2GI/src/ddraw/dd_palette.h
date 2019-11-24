#pragma once

#include "../common/common.h"
#include "../common/d3d7.h"


class PaletteProxy : public D3D7::IDirectDrawPalette, public Unknown
{
public:
	PaletteProxy();
	virtual ~PaletteProxy();

	STDMETHOD(QueryInterface) (REFIID riid, LPVOID FAR* ppvObj) { return Unknown::QueryInterface(riid, ppvObj); };
	STDMETHOD_(ULONG, AddRef) () { return Unknown::AddRef(); };
	STDMETHOD_(ULONG, Release) () { return Unknown::Release(); };

	STDMETHOD(GetCaps)(LPDWORD);
	STDMETHOD(GetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY);
	STDMETHOD(Initialize)(D3D7::LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
	STDMETHOD(SetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY);
};
