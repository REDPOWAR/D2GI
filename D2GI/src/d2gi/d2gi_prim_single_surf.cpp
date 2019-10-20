
#include "d2gi.h"
#include "d2gi_prim_single_surf.h"


D2GIPrimarySingleSurface::D2GIPrimarySingleSurface(D2GI* pD2GI) 
	: D2GISurface(pD2GI), m_pTexture(NULL), m_pSurface(NULL)
{

}


D2GIPrimarySingleSurface::~D2GIPrimarySingleSurface()
{
	ReleaseResource();
}


VOID D2GIPrimarySingleSurface::ReleaseResource()
{
	RELEASE(m_pSurface);
	RELEASE(m_pTexture);
}


VOID D2GIPrimarySingleSurface::LoadResource()
{
	m_dwWidth = m_pD2GI->GetOriginalWidth();
	m_dwHeight = m_pD2GI->GetOriginalHeight();
	m_dwBPP = m_pD2GI->GetOriginalBPP();

	if (m_dwBPP == 8)
	{
		D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

		pDev->CreateTexture(m_dwWidth, m_dwHeight, 1, D3DUSAGE_DYNAMIC,
			D3D9::D3DFMT_A8, D3D9::D3DPOOL_DEFAULT, &m_pTexture, NULL);

		m_pTexture->GetSurfaceLevel(0, &m_pSurface);
	}
}


HRESULT D2GIPrimarySingleSurface::Lock(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE)
{
	return DDERR_GENERIC;
}


HRESULT D2GIPrimarySingleSurface::Unlock(LPRECT)
{
	return DDERR_GENERIC;
}
