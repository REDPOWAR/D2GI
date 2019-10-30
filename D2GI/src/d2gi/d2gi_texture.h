#pragma once

#include "d2gi_surface.h"
#include "d2gi_mipmap_surf.h"


class D2GITexture : public D2GISurface
{
	BOOL m_bRenderTarget;
	DWORD m_dwWidth, m_dwHeight, m_dwMipMapCount;
	D2GIMipMapSurface** m_lpMipMapLevels;
	D3D9::IDirect3DTexture9* m_pTexture;
	D3D7::DDPIXELFORMAT m_sPixelFormat;

	D3D7::DDCOLORKEY m_sColorKey;
	DWORD m_dwCKFlags;
public:
	D2GITexture(D2GI*, DWORD dwW, DWORD dwH, DWORD dwMipMapCount, D3D7::DDPIXELFORMAT*);
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
	D3D9::IDirect3DTexture9* GetD3D9Texture() { return m_pTexture; }
	DWORD GetMipMapCount() { return m_dwMipMapCount; }
	VOID MakeRenderTarget();
	BOOL HasColorKey();
	DWORD GetColorKeyValue();
	DWORD GetOriginalColorKeyValue();
	DWORD GetWidth() { return m_dwWidth; }
	DWORD GetHeight() { return m_dwHeight; }
	D3D7::DDPIXELFORMAT* GetPixelFormat() { return &m_sPixelFormat; }
};
