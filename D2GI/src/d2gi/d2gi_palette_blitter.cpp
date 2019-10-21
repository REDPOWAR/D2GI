
#include "../common.h"

#include "d2gi_palette_blitter.h"
#include "d2gi_palette.h"


using namespace D3D9;


static CHAR* g_szVS = 
	"struct VS_OUT\n"
	"{\n"
	"	float4 vPos: POSITION;\n"
	"	float2 vScreenPos: TEXCOORD0;\n"
	"};\n"

	"VS_OUT main(float4 vPos: POSITION)\n"
	"{\n"
	"	VS_OUT sOut;\n"
	
	"	sOut.vPos = vPos;\n"
	"	sOut.vScreenPos = vPos.xy;\n"

	"	return sOut;\n"
	"}\n"

	"";


static CHAR* g_szPS =
	"sampler g_txIds : register(s0);\n"
	"sampler g_txPalette : register(s1);\n"

	"float4 g_vScreenRect : register(c0);\n"
	"float4 g_vTextureRect : register(c2);\n"

	"float4 main(float2 vScreenPos : TEXCOORD0) : COLOR0\n"
	"{\n"

	"	float2 vRealScreenPos = (vScreenPos + 1.0f) * 0.5f;\n"
	"	vRealScreenPos.y = 1.0f - vRealScreenPos.y;\n"

	"	float2 vTexPos = (vRealScreenPos - g_vScreenRect.xy) * g_vScreenRect.zw;\n"

	"	clip(vTexPos.x);\n"
	"	clip(vTexPos.y);\n"
	"	clip(-vTexPos.x + 1.0f);\n"
	"	clip(-vTexPos.y + 1.0f);\n"

	"	vTexPos = g_vTextureRect.xy + (vTexPos * g_vTextureRect.zw);\n"

	"	float idx = tex2D(g_txIds, vTexPos).a;\n"

	"	return tex2D(g_txPalette, float2( idx, 0.5f));\n"
	"}\n"

	"";


D2GIPaletteBlitter::D2GIPaletteBlitter(D2GI* pD2GI)
	: D2GIBase(pD2GI), m_pVDecl(NULL), m_pVB(NULL), m_pVS(NULL), m_pPS(NULL)
{

}


D2GIPaletteBlitter::~D2GIPaletteBlitter()
{
	ReleaseResource();
}


VOID D2GIPaletteBlitter::ReleaseResource()
{
	RELEASE(m_pVB);
	RELEASE(m_pVDecl);
	RELEASE(m_pVS);
	RELEASE(m_pPS);
}


VOID D2GIPaletteBlitter::LoadResource()
{
	D3DVERTEXELEMENT9 asVertexElements[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		D3DDECL_END()
	};
	IDirect3DDevice9* pDev = GetD3D9Device();

	pDev->CreateVertexDeclaration(asVertexElements, &m_pVDecl);

	FLOAT afVerts[] =
	{
		-1.0f, 1.0f, 0.0,
		1.0f, 1.0f, 0.0,
		-1.0f, -1.0f, 0.0,

		1.0f, 1.0f, 0.0,
		1.0f, -1.0f, 0.0,
		-1.0f, -1.0f, 0.0,
	};
	VOID* pData;

	pDev->CreateVertexBuffer(sizeof(afVerts), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	m_pVB->Lock(0, 0, &pData, 0);
	CopyMemory(pData, afVerts, sizeof(afVerts));
	m_pVB->Unlock();

	ID3DXBuffer* pBuf;

	D3DXCompileShader(g_szVS, strlen(g_szVS), NULL, NULL,
		"main", "vs_3_0", D3DXSHADER_OPTIMIZATION_LEVEL0, &pBuf, NULL, NULL);
	pDev->CreateVertexShader((DWORD*)pBuf->GetBufferPointer(), &m_pVS);
	pBuf->Release();

	D3DXCompileShader(g_szPS, strlen(g_szPS), NULL, NULL,
		"main", "ps_3_0", D3DXSHADER_OPTIMIZATION_LEVEL0, &pBuf, NULL, NULL);
	pDev->CreatePixelShader((DWORD*)pBuf->GetBufferPointer(), &m_pPS);
	pBuf->Release();
}


VOID D2GIPaletteBlitter::Blit(D3D9::IDirect3DSurface9* pDst, RECT* pDstRT, 
	D3D9::IDirect3DTexture9* pSrc, RECT* pSrcRT, D2GIPalette* pPalette)
{
	/*IDirect3DDevice9* pDev = GetD3D9Device();
	FLOAT afSrcRect[] = { 0.0, 0.0, 1.0f, 1.0f }, afDstRect[] = {0.0, 0.0, 1.0f, 1.0f};

	if (pSrcRT != NULL)
	{
		D3DSURFACE_DESC sDesc;

		pSrc->GetLevelDesc(0, &sDesc);
		afSrcRect[0] = (FLOAT)pSrcRT->left / (FLOAT)sDesc.Width;
		afSrcRect[1] = (FLOAT)pSrcRT->top / (FLOAT)sDesc.Height;
		afSrcRect[2] = (FLOAT)(pSrcRT->right - pSrcRT->left) / (FLOAT)sDesc.Width;
		afSrcRect[3] = (FLOAT)(pSrcRT->bottom - pSrcRT->top) / (FLOAT)sDesc.Height;
	}

	if (pDstRT != NULL)
	{
		D3DSURFACE_DESC sDesc;

		pDst->GetDesc(&sDesc);
		afDstRect[0] = (FLOAT)pDstRT->left / (FLOAT)sDesc.Width;
		afDstRect[1] = (FLOAT)pDstRT->top / (FLOAT)sDesc.Height;
		afDstRect[2] = (FLOAT)sDesc.Width / (FLOAT)(pDstRT->right - pDstRT->left);
		afDstRect[3] = (FLOAT)sDesc.Height / (FLOAT)(pDstRT->bottom - pDstRT->top);
	}

	pDev->SetVertexDeclaration(m_pVDecl);
	pDev->SetStreamSource(0, m_pVB, 0, sizeof(FLOAT) * 3);

	pDev->SetVertexShader(m_pVS);
	pDev->SetPixelShader(m_pPS);
	
	pDev->SetPixelShaderConstantF(0, afDstRect, 1);
	pDev->SetPixelShaderConstantF(1, afSrcRect, 1);

	pDev->SetTexture(0, pSrc);
	pDev->SetTexture(1, pPalette->GetD3D9Texture());
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
	pDev->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDev->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	pDev->SetRenderTarget(0, pDst);

	pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);*/
}
