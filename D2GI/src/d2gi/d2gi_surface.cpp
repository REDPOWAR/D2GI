

#include "../common/common.h"
#include "../common/logger.h"

#include "d2gi_surface.h"


using namespace D3D7;


D2GISurface::D2GISurface(D2GI* pD2GI, DWORD dwWidth, DWORD dwHeight, D2GIPIXELFORMAT eFormat) 
	: D2GIResource(pD2GI)
{
	m_dwWidth = dwWidth;
	m_dwHeight = dwHeight;
	m_eD2GIPixelFormat = eFormat;
	m_sDD7PixelFormat = g_asD2GIPF_To_DD7PF[eFormat];
	m_dwBPP = m_sDD7PixelFormat.dwRGBBitCount;
}


D2GISurface::~D2GISurface()
{
}


IFACEMETHODIMP D2GISurface::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	if (!(dwFlags & DDCKEY_SRCBLT))
	{
		Logger::Warning(TEXT("Setting unknown color key for texture"));
		return DDERR_GENERIC;
	}

	if (pCK != NULL)
	{
		m_sColorKey = *pCK;
		m_bColorKeySet = TRUE;
	}
	else
		m_bColorKeySet = FALSE;

	return DD_OK;
}


IFACEMETHODIMP D2GISurface::IsLost()
{
	return DD_OK;
}


D3D9::D3DFORMAT D2GISurface::GetEffectiveD3DFormat() const
{
	if (HasColorKeyConversion())
		return D3D9::D3DFMT_A8R8G8B8;
	if (m_dwBPP == 8)
		return D3D9::D3DFMT_R5G6B5;
	return g_asD2GIPF_To_D3D9PF[m_eD2GIPixelFormat];
}
