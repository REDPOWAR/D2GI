

#include "../common.h"

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
