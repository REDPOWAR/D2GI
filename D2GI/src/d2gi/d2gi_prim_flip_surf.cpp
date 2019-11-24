
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_backbuf_surf.h"


D2GIPrimaryFlippableSurface::D2GIPrimaryFlippableSurface(D2GI* pD2GI, 
	DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GISurface(pD2GI, dwWidth, dwHeight, eFormat)
{
	m_pBackBuffer = new D2GIBackBufferSurface(m_pD2GI, dwWidth, dwHeight, eFormat);
}


D2GIPrimaryFlippableSurface::~D2GIPrimaryFlippableSurface()
{
	DEL(m_pBackBuffer);
}


VOID D2GIPrimaryFlippableSurface::ReleaseResource()
{
	m_pBackBuffer->ReleaseResource();
}


VOID D2GIPrimaryFlippableSurface::LoadResource()
{
	m_pBackBuffer->LoadResource();
}


HRESULT D2GIPrimaryFlippableSurface::GetAttachedSurface(D3D7::LPDDSCAPS2 pCaps, D3D7::LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	if (pCaps->dwCaps & DDSCAPS_BACKBUFFER)
	{
		m_pBackBuffer->AddRef();
		*lpSurf = (D3D7::IDirectDrawSurface7*)m_pBackBuffer;
		return DD_OK;
	}

	Logger::Warning(TEXT("Requested unknown attached surface to primary flippable surface"));
	return DDERR_GENERIC;
}


HRESULT D2GIPrimaryFlippableSurface::GetSurfaceDesc(D3D7::LPDDSURFACEDESC2 pDesc)
{
	ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
	pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
	pDesc->dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_BACKBUFFERCOUNT;
	pDesc->dwBackBufferCount = 1;
	pDesc->dwWidth = m_dwWidth;
	pDesc->dwHeight = m_dwHeight;
	pDesc->lPitch = pDesc->dwWidth * m_dwBPP / 8;
	pDesc->ddsCaps.dwCaps = DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_FRONTBUFFER | DDSCAPS_PRIMARYSURFACE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VISIBLE | DDSCAPS_VIDEOMEMORY | DDSCAPS_3DDEVICE;
	pDesc->ddpfPixelFormat = m_sDD7PixelFormat;

	return DD_OK;
}


HRESULT D2GIPrimaryFlippableSurface::IsLost()
{
	return DD_OK;
}


HRESULT D2GIPrimaryFlippableSurface::Flip(D3D7::LPDIRECTDRAWSURFACE7 pSurf, DWORD dwFlags)
{
	m_pD2GI->OnFlip();
	return DD_OK;
}


HRESULT D2GIPrimaryFlippableSurface::GetFlipStatus(DWORD)
{
	return DD_OK;
}
