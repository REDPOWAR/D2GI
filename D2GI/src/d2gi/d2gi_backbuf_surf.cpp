
#include "d2gi_backbuf_surf.h"
#include "d2gi.h"
#include "d2gi_enums.h"


D2GIBackBufferSurface::D2GIBackBufferSurface(D2GI* pD2GI) 
	: D2GISurface(pD2GI), m_pData(NULL)
{
	LoadResource();
}


D2GIBackBufferSurface::~D2GIBackBufferSurface()
{
	ReleaseResource();
}


VOID D2GIBackBufferSurface::ReleaseResource()
{
	DEL(m_pData);
}


VOID D2GIBackBufferSurface::LoadResource()
{
	m_dwWidth = m_pD2GI->GetOriginalWidth();
	m_dwHeight = m_pD2GI->GetOriginalHeight();
	m_dwBPP = m_pD2GI->GetOriginalBPP();
	m_uDataSize = m_dwWidth * m_dwHeight * m_dwBPP / 8;
	m_uPitch = m_dwWidth * m_dwBPP / 8;
	m_pData = new BYTE[m_uDataSize];
	if (m_dwBPP == 16)
		m_sPixelFormat = g_pf16_565;
}


HRESULT D2GIBackBufferSurface::Lock(LPRECT pRect, D3D7::LPDDSURFACEDESC2 pDesc, DWORD, HANDLE)
{
	if (pRect == NULL)
	{
		ZeroMemory(pDesc, sizeof(D3D7::DDSURFACEDESC2));
		pDesc->dwSize = sizeof(D3D7::DDSURFACEDESC2);
		pDesc->dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT | DDSD_LPSURFACE;
		pDesc->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER | DDSCAPS_COMPLEX | DDSCAPS_FLIP | DDSCAPS_3DDEVICE | DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY;
		pDesc->dwWidth = m_dwWidth;
		pDesc->dwHeight = m_dwHeight;
		pDesc->ddpfPixelFormat = m_sPixelFormat;
		pDesc->lPitch = m_uPitch;
		pDesc->lpSurface = m_pData;

		return DD_OK;
	}

	return DDERR_GENERIC;
}


HRESULT D2GIBackBufferSurface::Unlock(LPRECT)
{
	return DD_OK;
}
