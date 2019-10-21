#pragma once

#include "../ddraw/dd_palette.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"


class D2GIPalette : public PaletteProxy, public D2GIResource
{
	PALETTEENTRY m_asEntries[256];
	UINT16 m_auEntries16[256];

	VOID UpdateEntries16(DWORD dwIdx, DWORD dwCount);
public:
	D2GIPalette(D2GI*, PALETTEENTRY*);
	virtual ~D2GIPalette();

	virtual VOID ReleaseResource() {};
	virtual VOID LoadResource() {};

	STDMETHOD(SetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY);

	PALETTEENTRY* GetEntries() { return m_asEntries; }
	UINT16* GetEntries16() { return m_auEntries16; }
};
