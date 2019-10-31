
#include "d2gi.h"
#include "d2gi_prim_single_surf.h"
#include "d2gi_enums.h"


D2GIPrimarySingleSurface::D2GIPrimarySingleSurface(D2GI* pD2GI,
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat)
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	m_pPalette = NULL;
}


D2GIPrimarySingleSurface::~D2GIPrimarySingleSurface()
{
	RELEASE(m_pPalette);
}


HRESULT D2GIPrimarySingleSurface::GetSurfaceDesc(D3D7::LPDDSURFACEDESC2 pDesc)
{

	ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
	pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
	pDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PITCH | DDSD_PIXELFORMAT;
	pDesc->dwWidth = m_dwWidth;
	pDesc->dwHeight = m_dwHeight;
	pDesc->lPitch = pDesc->dwWidth * m_dwBPP / 8;
	pDesc->ddsCaps.dwCaps = DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VISIBLE | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;
	pDesc->ddpfPixelFormat = m_sDD7PixelFormat;

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
