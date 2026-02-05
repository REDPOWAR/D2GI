#pragma once

#include "../ddraw/dd_palette.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"

#include "../common/implements_proxy.hpp"

class D2GIPalette : public ImplementsProxy<PaletteProxy>, public D2GIResource
{
private:
	UINT16 m_auEntries16[256];

	void UpdateEntries16(const PALETTEENTRY* pEntries, DWORD dwIdx, DWORD dwCount);

public:
	D2GIPalette(D2GI*, PALETTEENTRY*);
	virtual ~D2GIPalette() override;

	virtual VOID ReleaseResource() override {};
	virtual VOID LoadResource() override {};

	STDMETHOD(SetEntries)(DWORD, DWORD, DWORD, LPPALETTEENTRY) override;

	const UINT16* GetEntries16() const { return m_auEntries16; }
};
