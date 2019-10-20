
#include "d2gi.h"
#include "d2gi_prim_single_surf.h"
#include "d2gi_enums.h"


D2GIPrimarySingleSurface::D2GIPrimarySingleSurface(D2GI* pD2GI) 
	: D2GISurface(pD2GI), m_pPalette(NULL)
{

}


D2GIPrimarySingleSurface::~D2GIPrimarySingleSurface()
{
	RELEASE(m_pPalette);
}


HRESULT D2GIPrimarySingleSurface::GetSurfaceDesc(D3D7::LPDDSURFACEDESC2 pDesc)
{
	DWORD dwBPP = m_pD2GI->GetOriginalBPP();

	ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
	pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
	pDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PITCH | DDSD_PIXELFORMAT;
	pDesc->dwWidth = m_pD2GI->GetOriginalWidth();
	pDesc->dwHeight = m_pD2GI->GetOriginalHeight();
	pDesc->lPitch = pDesc->dwWidth * dwBPP / 8;
	pDesc->ddsCaps.dwCaps = DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VISIBLE | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;

	if (dwBPP == 8)
		pDesc->ddpfPixelFormat = g_pf8_Pal;
	else if (dwBPP == 16)
		pDesc->ddpfPixelFormat = g_pf16_565;

	return DD_OK;
}


HRESULT D2GIPrimarySingleSurface::SetPalette(D3D7::LPDIRECTDRAWPALETTE pPal)
{
	D2GIPalette* pD2GIPalette = (D2GIPalette*)pPal;

	if (m_pPalette == pPal)
		return DD_OK;

	RELEASE(m_pPalette);
	m_pPalette = pD2GIPalette;
	m_pPalette->AddRef();

	return DD_OK;
}



HRESULT D2GIPrimarySingleSurface::IsLost()
{
	return DD_OK;
}


HRESULT D2GIPrimarySingleSurface::Blt(LPRECT pDestRT, D3D7::LPDIRECTDRAWSURFACE7 pSrc, LPRECT pSrcRT, DWORD dwFlags, D3D7::LPDDBLTFX lpFX)
{
	D2GISurface* pSurf = (D2GISurface*)pSrc;

	if (pSrc == NULL || pSurf->GetType() != ST_SYSMEM)
		return DDERR_GENERIC;

	m_pD2GI->OnSysMemSurfaceBltOnPrimarySingle((D2GISystemMemorySurface*)pSurf, pSrcRT, this, pDestRT);

	return DD_OK;
}

