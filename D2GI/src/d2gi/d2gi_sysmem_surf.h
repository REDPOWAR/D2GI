#pragma once

#include "d2gi_surface.h"


class D2GIPalette;


class D2GISystemMemorySurface : public D2GISurface
{
	DWORD m_dwWidth, m_dwHeight, m_dwBPP;
	D3D7::DDPIXELFORMAT m_sPixelFormat;

	D3D9::IDirect3DTexture9* m_pTexture;
	D3D9::IDirect3DSurface9* m_pSurface;

	VOID* m_pData;
	UINT m_uDataSize, m_uPitch;

	D3D7::DDCOLORKEY m_sColorKey;
	DWORD m_dwCKFlags;
public:
	D2GISystemMemorySurface(D2GI*, DWORD dwW, DWORD dwH, D3D7::DDPIXELFORMAT*);
	virtual ~D2GISystemMemorySurface();

	virtual SURFACETYPE GetType() { return ST_SYSMEM; }
	virtual VOID ReleaseResource();
	virtual VOID LoadResource();

	STDMETHOD(SetColorKey)(DWORD, D3D7::LPDDCOLORKEY);
	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(IsLost)();
	STDMETHOD(GetCaps)(D3D7::LPDDSCAPS2);

	D3D9::IDirect3DSurface9* GetD3D9Surface() { return m_pSurface; }
	D3D9::IDirect3DTexture9* GetD3D9Texture() { return m_pTexture; }
	VOID UpdateWithPalette(D2GIPalette*);
	BOOL HasColorKey();
	DWORD GetColorKeyValue();
	VOID* GetDataSize();
	VOID* GetData() { return m_pData; }
	UINT GetDataPitch() { return m_uPitch; }
	DWORD GetWidth() { return m_dwWidth; }
	DWORD GetHeight() { return m_dwHeight; }
};
