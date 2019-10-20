
#include "d2gi_palette.h"


D2GIPalette::D2GIPalette(D2GI* pD2GI, PALETTEENTRY* pEntries) 
	: PaletteProxy(), D2GIResource(pD2GI), m_pTexture(NULL)
{
	CopyMemory(m_asEntries, pEntries, sizeof(m_asEntries));
	LoadResource();
}


D2GIPalette::~D2GIPalette()
{
	ReleaseResource();
}


HRESULT D2GIPalette::SetEntries(DWORD, DWORD dwIdx, DWORD dwCount, LPPALETTEENTRY pEntries)
{
	CopyMemory(m_asEntries + dwIdx, pEntries, sizeof(PALETTEENTRY) * dwCount);
	ReleaseResource();
	LoadResource();

	return DD_OK;
}


VOID D2GIPalette::ReleaseResource()
{
	RELEASE(m_pTexture);
}


VOID D2GIPalette::LoadResource()
{
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::IDirect3DSurface9* pSurf;
	D3D9::D3DLOCKED_RECT sLockedRect;
	INT i;

	pDev->CreateTexture(256, 1, 1, D3DUSAGE_DYNAMIC, D3D9::D3DFMT_A8R8G8B8, 
		D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL);

	m_pTexture->GetSurfaceLevel(0, &pSurf);
	pSurf->LockRect(&sLockedRect, NULL, 0);
	for (i = 0; i < 256; i++)
		((UINT32*)sLockedRect.pBits)[i] = m_asEntries[i].peRed << 16 | m_asEntries[i].peGreen << 8 | m_asEntries[i].peBlue;
 
	pSurf->UnlockRect();
	pSurf->Release();
}
