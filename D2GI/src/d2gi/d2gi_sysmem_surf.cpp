
#include "d2gi_sysmem_surf.h"


D2GISystemMemorySurface::D2GISystemMemorySurface(D2GI* pD2GI, DWORD dwW, DWORD dwH) 
	: D2GISurface(pD2GI), m_dwWidth(dwW), m_dwHeight(dwH), m_dwCKFlags(0)
{

}


D2GISystemMemorySurface::~D2GISystemMemorySurface()
{

}



HRESULT D2GISystemMemorySurface::SetColorKey(DWORD dwFlags, D3D7::LPDDCOLORKEY pCK)
{
	m_dwCKFlags = dwFlags;
	m_sColorKey = *pCK;

	return DD_OK;
}
