
#include "../common.h"

#include "d2gi_blitter.h"


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
	"sampler g_txSourceTexture : register(s0);\n"

	"float4 g_vTextureRect : register(c0);\n"

	"float4 main(float2 vScreenPos : TEXCOORD0) : COLOR0\n"
	"{\n"

	"	float2 vRealScreenPos = (vScreenPos + 1.0f) * 0.5f;\n"
	"	vRealScreenPos.y = 1.0f - vRealScreenPos.y;\n"

	"	float2 vTexPos = g_vTextureRect.xy + (vRealScreenPos * g_vTextureRect.zw);\n"

	"	return tex2D(g_txSourceTexture, vTexPos).rgba;\n"
	"}\n"

	"";


D2GIBlitter::D2GIBlitter(D2GI* pD2GI)
	: D2GIBase(pD2GI), m_pVDecl(NULL), m_pVB(NULL), m_pVS(NULL), m_pPS(NULL)
{

}


D2GIBlitter::~D2GIBlitter()
{
	ReleaseResource();
}


VOID D2GIBlitter::ReleaseResource()
{
	RELEASE(m_pVB);
	RELEASE(m_pVDecl);
	RELEASE(m_pVS);
	RELEASE(m_pPS);
}


VOID D2GIBlitter::LoadResource()
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


VOID D2GIBlitter::Blit(D3D9::IDirect3DSurface9* pDst, RECT* pDstRT,
	D3D9::IDirect3DTexture9* pSrc, RECT* pSrcRT)
{
	IDirect3DDevice9* pDev = GetD3D9Device();
	FLOAT afSrcRect[] = { 0.0, 0.0, 1.0f, 1.0f };
	D3DVIEWPORT9 sOriginalVP, sUsedVP;
	DWORD dwZEnable, dwZWriteEnable;
	D3DSURFACE_DESC sDstDesc;
	IDirect3DBaseTexture9* pCurrentTexture1 = NULL, *pCurrentTexture2 = NULL;
	DWORD dwMinFilter, dwMagFilter;

	pDst->GetDesc(&sDstDesc);

	pDev->GetViewport(&sOriginalVP);
	pDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	pDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWriteEnable);
	pDev->GetTexture(0, &pCurrentTexture1);
	pDev->GetTexture(1, &pCurrentTexture2);
	pDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
	pDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);

	if (pSrcRT != NULL)
	{
		D3DSURFACE_DESC sDesc;

		pSrc->GetLevelDesc(0, &sDesc);
		afSrcRect[0] = (FLOAT)pSrcRT->left / (FLOAT)sDesc.Width;
		afSrcRect[1] = (FLOAT)pSrcRT->top / (FLOAT)sDesc.Height;
		afSrcRect[2] = (FLOAT)(pSrcRT->right - pSrcRT->left) / (FLOAT)sDesc.Width;
		afSrcRect[3] = (FLOAT)(pSrcRT->bottom - pSrcRT->top) / (FLOAT)sDesc.Height;
	}

	pDev->SetRenderTarget(0, pDst);

	pDev->SetVertexDeclaration(m_pVDecl);
	pDev->SetStreamSource(0, m_pVB, 0, sizeof(FLOAT) * 3);

	pDev->SetVertexShader(m_pVS);
	pDev->SetPixelShader(m_pPS);
	
	pDev->SetPixelShaderConstantF(0, afSrcRect, 1);

	pDev->SetTexture(0, pSrc);
	pDev->SetTexture(1, NULL);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	if (pDstRT != NULL)
	{
		sUsedVP.X = pDstRT->left;
		sUsedVP.Y = pDstRT->top;
		sUsedVP.Width = pDstRT->right - pDstRT->left;
		sUsedVP.Height = pDstRT->bottom - pDstRT->top;
	}
	else
	{
		sUsedVP.X = sUsedVP.Y = 0;
		sUsedVP.Width = sDstDesc.Width;
		sUsedVP.Height = sDstDesc.Height;
	}
	sUsedVP.MinZ = 0.0; sUsedVP.MaxZ = 1.0f;
	pDev->SetViewport(&sUsedVP);
	pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);

	pDev->SetPixelShader(NULL);
	pDev->SetVertexShader(NULL);
	pDev->SetTexture(0, pCurrentTexture1);
	pDev->SetTexture(1, pCurrentTexture2);
	RELEASE(pCurrentTexture1);
	RELEASE(pCurrentTexture2);
	pDev->SetViewport(&sOriginalVP);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
	pDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWriteEnable);
}
