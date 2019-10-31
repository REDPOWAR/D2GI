#pragma once

#include "d2gi_surface.h"


class D2GITexture;


class D2GIMipMapSurface : public D2GISurface
{
	D2GITexture* m_pParent;
	D2GIMipMapSurface* m_pNextLevel;
	UINT m_uLevelID;

	D3D9::IDirect3DSurface9* m_pSurface;
	VOID* m_pData;
	UINT m_uDataSize;

public:
	D2GIMipMapSurface(D2GITexture*, UINT, D2GIMipMapSurface*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GIMipMapSurface();

	virtual SURFACETYPE GetType() { return ST_MIPMAP; }
	virtual VOID ReleaseResource() {};
	virtual VOID LoadResource() {};

	VOID SetD3D9Surface(D3D9::IDirect3DSurface9*);
	D3D9::IDirect3DSurface9* GetD3D9Surface() { return m_pSurface; }

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(GetAttachedSurface)(D3D7::LPDDSCAPS2, D3D7::LPDIRECTDRAWSURFACE7 FAR*);
	VOID UpdateSurface();
};
