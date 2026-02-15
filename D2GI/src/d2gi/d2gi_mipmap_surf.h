#pragma once

#include "d2gi_surface.h"

#include <wrl/client.h>

class D2GITexture;
class D2GIPalette;


class D2GIMipMapSurface : public D2GISurface
{
	D2GITexture* m_pParent = nullptr;
	D2GIMipMapSurface* m_pNextLevel = nullptr;
	UINT m_uLevelID = 0;

	Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> m_pSurface;

public:
	D2GIMipMapSurface(D2GITexture*, UINT, D2GIMipMapSurface*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GIMipMapSurface();

	virtual SURFACETYPE GetType() const override { return ST_MIPMAP; }
	virtual void ReleaseResource(bool bResettingDevice) override;
	virtual void LoadResource(bool bResettingDevice) override {};

	void SetD3D9Surface(Microsoft::WRL::ComPtr<D3D9::IDirect3DSurface9> pSurf);
	D3D9::IDirect3DSurface9* GetD3D9Surface() const { return m_pSurface.Get(); }

	STDMETHOD(Lock)(LPRECT, D3D7::LPDDSURFACEDESC2, DWORD, HANDLE);
	STDMETHOD(Unlock)(LPRECT);
	STDMETHOD(GetAttachedSurface)(D3D7::LPDDSCAPS2, D3D7::LPDIRECTDRAWSURFACE7 FAR*);

	void FlushResourceFromParent() { FlushResourceToGPU(); }

protected:
	void FlushResourceToGPU();
};
