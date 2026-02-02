#pragma once

#include "d2gi_common.h"

#include <wrl/client.h>


class D2GIBlitter : public D2GIBase
{
	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexDeclaration9> m_pVDecl;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexBuffer9> m_pVB;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DVertexShader9> m_pVS;
	Microsoft::WRL::ComPtr<D3D9::IDirect3DPixelShader9> m_pPS;
public:
	using D2GIBase::D2GIBase;

	void ReleaseResource();
	void LoadResource();
;
	void Blit(D3D9::IDirect3DSurface9*, const FRECT*, D3D9::IDirect3DTexture9*, const FRECT*, bool bEmulateCK);
};
