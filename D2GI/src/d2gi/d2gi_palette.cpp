
#include "d2gi_palette.h"


D2GIPalette::D2GIPalette(D2GI* pD2GI, PALETTEENTRY* pEntries) 
	: D2GIResource(pD2GI)
{
	UpdateEntries16(pEntries, 0, 256);
}


D2GIPalette::~D2GIPalette()
{
}


HRESULT D2GIPalette::SetEntries(DWORD, DWORD dwIdx, DWORD dwCount, LPPALETTEENTRY pEntries)
{
	UpdateEntries16(pEntries, dwIdx, dwCount);

	return DD_OK;
}


void D2GIPalette::UpdateEntries16(const PALETTEENTRY* pEntries, DWORD dwIdx, DWORD dwCount)
{
	for (DWORD i = dwIdx; i < dwIdx + dwCount; i++)
	{
		BYTE bR = (BYTE)((INT)pEntries[i].peRed * 31 / 255);
		BYTE bG = (BYTE)((INT)pEntries[i].peGreen * 63 / 255);
		BYTE bB = (BYTE)((INT)pEntries[i].peBlue * 31 / 255);

		m_auEntries16[i] = (UINT16)((bR << 11) | (bG << 5) | (bB));
	}
}
