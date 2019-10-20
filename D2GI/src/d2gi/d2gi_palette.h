#pragma once

#include "../ddraw/dd_palette.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"


class D2GIPalette : public PaletteProxy, public D2GIResource
{
	PALETTEENTRY m_asEntries[256];
	D3D9::IDirect3DTexture9* m_pTexture;

public:
	D2GIPalette(D2GI*, PALETTEENTRY*);
	virtual ~D2GIPalette();

	virtual VOID ReleaseResource();
	virtual VOID LoadResource();

	STDMETHOD(SetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY);

	D3D9::IDirect3DTexture9* GetD3D9Texture() { return m_pTexture; }
};
