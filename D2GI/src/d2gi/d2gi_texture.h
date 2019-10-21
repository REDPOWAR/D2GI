#pragma once

#include "d2gi_surface.h"


class D2GITexture : public D2GISurface
{
	DWORD m_dwWidth, m_dwHeight, m_dwMipMapCount;
	D3D9::IDirect3DTexture9* m_pTexture;
	D3D9::IDirect3DSurface9* m_pSurface;

	D3D7::DDCOLORKEY m_sColorKey;
	DWORD m_dwCKFlags;
public:
	D2GITexture(D2GI*, DWORD dwW, DWORD dwH, DWORD dwMipMapCount);
	virtual ~D2GITexture();

	virtual SURFACETYPE GetType() { return ST_TEXTURE; }
	virtual VOID LoadResource();
	virtual VOID ReleaseResource();

	STDMETHOD(SetColorKey)(DWORD, D3D7::LPDDCOLORKEY);
	STDMETHOD(IsLost)();
	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(Blt)(LPRECT, D3D7::LPDIRECTDRAWSURFACE7, LPRECT, DWORD, D3D7::LPDDBLTFX);

	D3D9::IDirect3DSurface9* GetD3D9Surface() { return m_pSurface; }
};
