
#include "d2gi_palette.h"


D2GIPalette::D2GIPalette(D2GI* pD2GI, PALETTEENTRY* pEntries) 
	: PaletteProxy(), D2GIResource(pD2GI)
{
	CopyMemory(m_asEntries, pEntries, sizeof(m_asEntries));
	UpdateEntries16(0, 256);
}


D2GIPalette::~D2GIPalette()
{
}


HRESULT D2GIPalette::SetEntries(DWORD, DWORD dwIdx, DWORD dwCount, LPPALETTEENTRY pEntries)
{
	CopyMemory(m_asEntries + dwIdx, pEntries, sizeof(PALETTEENTRY) * dwCount);
	UpdateEntries16(dwIdx, dwCount);

	return DD_OK;
}


VOID D2GIPalette::UpdateEntries16(DWORD dwIdx, DWORD dwCount)
{
	DWORD i;

	for (i = dwIdx; i < dwIdx + dwCount; i++)
	{
		BYTE bR = (BYTE)((INT)m_asEntries[i].peRed * (1 << 5) / 255);
		BYTE bG = (BYTE)((INT)m_asEntries[i].peGreen * (1 << 6) / 255);
		BYTE bB = (BYTE)((INT)m_asEntries[i].peBlue * (1 << 5) / 255);

		m_auEntries16[i] = (UINT16)((bR << 11) | (bG << 5) | (bB));
	}
}
