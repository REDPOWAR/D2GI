

#include "../common.h"

#include "d2gi_surface.h"


using namespace D3D7;


D2GISurface::D2GISurface(IDirectDrawSurface7* pOriginal) 
	: SurfaceProxy(pOriginal), m_pBackBuffer(NULL), m_pZBuffer(NULL), m_pMipMap(NULL)
{
	DDSCAPS2 sCaps;

	ZeroMemory(&sCaps, sizeof(sCaps));
	m_pOriginal->GetCaps(&sCaps);

	if ((sCaps.dwCaps & DDSCAPS_FLIP) && (sCaps.dwCaps & DDSCAPS_PRIMARYSURFACE))
	{
		IDirectDrawSurface7* pBB;
		DDSCAPS2 sBBCaps;

		ZeroMemory(&sBBCaps, sizeof(sBBCaps));
		sBBCaps.dwCaps = DDSCAPS_BACKBUFFER;
		m_pOriginal->GetAttachedSurface(&sBBCaps, &pBB);

		m_pBackBuffer = new D2GISurface(pBB);
	}

	if (sCaps.dwCaps & DDSCAPS_MIPMAP)
	{
		IDirectDrawSurface7* pMipMap;
		DDSCAPS2 sMMCaps;

		ZeroMemory(&sMMCaps, sizeof(sMMCaps));
		sMMCaps.dwCaps = DDSCAPS_MIPMAP | DDSCAPS_TEXTURE;
		if(SUCCEEDED(m_pOriginal->GetAttachedSurface(&sMMCaps, &pMipMap)))
			m_pMipMap = new D2GISurface(pMipMap);
	}
}


D2GISurface::~D2GISurface()
{
	DEL(m_pBackBuffer); // delete instead of Release, because back buffer will be destroyed automatically
	DEL(m_pZBuffer);
	DEL(m_pMipMap);
}


HRESULT D2GISurface::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpSrc, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpFX)
{
	if (lpSrc != NULL)
		lpSrc = ((D2GISurface*)lpSrc)->GetOriginal();
	Debug(TEXT("Blitting 0x%08x to 0x%08x"), lpSrc, GetOriginal());
	return SurfaceProxy::Blt(lpDestRect, lpSrc, lpSrcRect, dwFlags, lpFX);
}



HRESULT D2GISurface::Flip(LPDIRECTDRAWSURFACE7 lpSurf, DWORD dwFlags)
{
	Debug(TEXT("Flip (0x%08x)"), lpSurf);
	if (lpSurf != NULL)
		lpSurf = ((D2GISurface*)lpSurf)->GetOriginal();
	return SurfaceProxy::Flip(lpSurf, dwFlags);
}


HRESULT D2GISurface::GetAttachedSurface(LPDDSCAPS2 lpCaps, LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	Debug(TEXT("Getting attached surface 0x%08x with flags %i"), *lpSurf, lpCaps->dwCaps);
	
	if (lpCaps->dwCaps & DDSCAPS_BACKBUFFER)
	{
		m_pBackBuffer->AddRef();
		*lpSurf = (IDirectDrawSurface7*)m_pBackBuffer;
		return DD_OK;
	}

	if ((lpCaps->dwCaps & DDSCAPS_MIPMAP) && (lpCaps->dwCaps & DDSCAPS_TEXTURE))
	{
		m_pMipMap->AddRef();
		*lpSurf = (IDirectDrawSurface7*)m_pMipMap;
		return DD_OK;
	}

	return DD_FALSE;
}


HRESULT D2GISurface::AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpSurf)
{
	DDSURFACEDESC2 sDesc;

	ZeroMemory(&sDesc, sizeof(sDesc));
	sDesc.dwSize = sizeof(sDesc);
	sDesc.dwFlags = DDSD_CAPS;

	lpSurf->GetSurfaceDesc(&sDesc);
	if (sDesc.ddsCaps.dwCaps & DDSCAPS_ZBUFFER)
	{
		D2GISurface* pZBuf = ((D2GISurface*)lpSurf);

		Debug(TEXT("Attaching z-buffer 0x%08x to 0x%08x"), pZBuf->GetOriginal(), GetOriginal());

		HRESULT hRes = SurfaceProxy::AddAttachedSurface(pZBuf->GetOriginal());

		if (SUCCEEDED(hRes))
		{
			pZBuf->AddRef();
			m_pZBuffer = pZBuf;
		}

		return hRes;
	}

	return DD_FALSE;
}