#pragma once

#include "../common/d3d7.h"


class PaletteProxy : public D3D7::IDirectDrawPalette
{
public:
	static inline const GUID proxy_guid = D3D7::IID_IDirectDrawPalette;
	using proxy_type = D3D7::IDirectDrawPalette;

public:
	STDMETHOD(GetCaps)(LPDWORD);
	STDMETHOD(GetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY);
	STDMETHOD(Initialize)(D3D7::LPDIRECTDRAW, DWORD, LPPALETTEENTRY);
	STDMETHOD(SetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY);
};
