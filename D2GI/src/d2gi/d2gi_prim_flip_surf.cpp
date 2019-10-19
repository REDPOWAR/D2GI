
#include "d2gi.h"
#include "d2gi_prim_flip_surf.h"


D2GIPrimaryFlippableSurface::D2GIPrimaryFlippableSurface(D2GI* pD2GI) : D2GISurface(pD2GI)
{

}


D2GIPrimaryFlippableSurface::~D2GIPrimaryFlippableSurface()
{

}


HRESULT D2GIPrimaryFlippableSurface::GetAttachedSurface(D3D7::LPDDSCAPS2 pCaps, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT D2GIPrimaryFlippableSurface::GetSurfaceDesc(D3D7::LPDDSURFACEDESC2 pDesc)
{
	ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
	pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
	pDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_BACKBUFFERCOUNT;
	pDesc->dwBackBufferCount = 1;
	pDesc->dwWidth = m_pD2GI->GetOriginalWidth();
	pDesc->dwHeight = m_pD2GI->GetOriginalHeight();
	pDesc->lPitch = pDesc->dwWidth * m_pD2GI->GetOriginalBPP() / 8;
	pDesc->ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VISIBLE | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;
	pDesc->ddpfPixelFormat.dwSize = sizeof(D3D7::DDPIXELFORMAT);
	pDesc->ddpfPixelFormat.dwFlags = DDPF_RGB;
	pDesc->ddpfPixelFormat.dwRGBBitCount = m_pD2GI->GetOriginalBPP();

	return DD_OK;
}
