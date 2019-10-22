#pragma once

#include "d2gi_surface.h"
#include "d2gi_mipmap_surf.h"


class D2GITexture : public D2GISurface
{
	DWORD m_dwWidth, m_dwHeight, m_dwMipMapCount;
	D2GIMipMapSurface** m_lpMipMapLevels;
	D3D9::IDirect3DTexture9* m_pTexture;

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
	STDMETHOD(GetAttachedSurface)(D3D7::LPDDSCAPS2, D3D7::LPDIRECTDRAWSURFACE7 FAR*);
	STDMETHOD(GetSurfaceDesc)(D3D7::LPDDSURFACEDESC2);

	D3D9::IDirect3DSurface9* GetD3D9Surface();
	DWORD GetMipMapCount() { return m_dwMipMapCount; }
};