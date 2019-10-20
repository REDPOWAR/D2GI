#pragma once

#include "d2gi_common.h"


class D2GIPalette;


class D2GIPaletteBlitter : public D2GIBase
{
	D3D9::IDirect3DVertexDeclaration9* m_pVDecl;
	D3D9::IDirect3DVertexBuffer9* m_pVB;
	D3D9::IDirect3DVertexShader9* m_pVS;
	D3D9::IDirect3DPixelShader9* m_pPS;
public:
	D2GIPaletteBlitter(D2GI*);
	~D2GIPaletteBlitter();

	VOID ReleaseResource();
	VOID LoadResource();

	VOID Blit(D3D9::IDirect3DSurface9*, RECT*, D3D9::IDirect3DTexture9*, RECT*, D2GIPalette*);
};
