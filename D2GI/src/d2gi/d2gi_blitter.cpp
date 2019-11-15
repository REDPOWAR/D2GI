
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
	"float2 g_vScreenPosBias: register(c1);\n"

	"float4 main(float2 vScreenPos : TEXCOORD0) : COLOR0\n"
	"{\n"

	"	float2 vRealScreenPos = (vScreenPos + 1.0f) * 0.5f;\n"
	"	vRealScreenPos.y = 1.0f - vRealScreenPos.y;\n"
	"	vRealScreenPos += g_vScreenPosBias;\n"

	"	float2 vTexPos = g_vTextureRect.xy + (vRealScreenPos * g_vTextureRect.zw);\n"

	"	return tex2D(g_txSourceTexture, vTexPos).rgba;\n"
	"}\n"

	"";


static CHAR* g_szPSCK =
	"sampler g_txSourceTexture : register(s0);\n"

	"float4 g_vTextureRect : register(c0);\n"
	"float2 g_vScreenPosBias: register(c1);\n"
	"float4 g_clColorKey: register(c2);\n"

	"float4 main(float2 vScreenPos : TEXCOORD0) : COLOR0\n"
	"{\n"

	"	float2 vRealScreenPos = (vScreenPos + 1.0f) * 0.5f;\n"
	"	vRealScreenPos.y = 1.0f - vRealScreenPos.y;\n"
	"	vRealScreenPos += g_vScreenPosBias;\n"

	"	float2 vTexPos = g_vTextureRect.xy + (vRealScreenPos * g_vTextureRect.zw);\n"

	"	float4 sample = tex2D(g_txSourceTexture, vTexPos).rgba;\n"

	"	clip(length(sample - g_clColorKey) - 0.000001f);"

	"	return sample;\n"
	"}\n"

	"";


D2GIBlitter::D2GIBlitter(D2GI* pD2GI)
	: D2GIBase(pD2GI), m_pVDecl(NULL), m_pVB(NULL), m_pVS(NULL), m_pPS(NULL), m_pPSCK(NULL)
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
	RELEASE(m_pPSCK);
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

	D3DXCompileShader(g_szPSCK, strlen(g_szPSCK), NULL, NULL,
		"main", "ps_3_0", D3DXSHADER_OPTIMIZATION_LEVEL0, &pBuf, NULL, NULL);
	pDev->CreatePixelShader((DWORD*)pBuf->GetBufferPointer(), &m_pPSCK);
	pBuf->Release();
}


VOID D2GIBlitter::Blit(D3D9::IDirect3DSurface9* pDst, RECT* pDstRT,
	D3D9::IDirect3DTexture9* pSrc, RECT* pSrcRT)
{
	IDirect3DDevice9* pDev = GetD3D9Device();
	FLOAT afSrcRect[] = { 0.0, 0.0, 1.0f, 1.0f }, afBiasRect[] = { 0.0, 0.0, 0.0f, 0.0f };
	D3DVIEWPORT9 sOriginalVP, sUsedVP;
	DWORD dwZEnable, dwZWriteEnable;
	D3DSURFACE_DESC sDstDesc, sSrcDesc;
	IDirect3DBaseTexture9* pCurrentTexture1 = NULL, *pCurrentTexture2 = NULL;
	DWORD dwMinFilter, dwMagFilter, dwCullMode, dwAlphaBlending;

	pDst->GetDesc(&sDstDesc);
	pSrc->GetLevelDesc(0, &sSrcDesc);

	pDev->GetViewport(&sOriginalVP);
	pDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	pDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWriteEnable);
	pDev->GetTexture(0, &pCurrentTexture1);
	pDev->GetTexture(1, &pCurrentTexture2);
	pDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
	pDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
	pDev->GetRenderState(D3DRS_CULLMODE, &dwCullMode);
	pDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlending);

	if (pSrcRT != NULL)
	{
		D3DSURFACE_DESC ;

		afSrcRect[0] = (FLOAT)pSrcRT->left / (FLOAT)sSrcDesc.Width;
		afSrcRect[1] = (FLOAT)pSrcRT->top / (FLOAT)sSrcDesc.Height;
		afSrcRect[2] = (FLOAT)(pSrcRT->right - pSrcRT->left) / (FLOAT)sSrcDesc.Width;
		afSrcRect[3] = (FLOAT)(pSrcRT->bottom - pSrcRT->top) / (FLOAT)sSrcDesc.Height;
	}

	afBiasRect[0] = 0.5f / (FLOAT)(pDstRT->right - pDstRT->left);
	afBiasRect[1] = 0.5f / (FLOAT)(pDstRT->bottom - pDstRT->top);

	pDev->SetRenderTarget(0, pDst);

	pDev->SetVertexDeclaration(m_pVDecl);
	pDev->SetStreamSource(0, m_pVB, 0, sizeof(FLOAT) * 3);

	pDev->SetVertexShader(m_pVS);
	pDev->SetPixelShader(m_pPS);
	
	pDev->SetPixelShaderConstantF(0, afSrcRect, 1);
	pDev->SetPixelShaderConstantF(1, afBiasRect, 1);

	pDev->SetTexture(0, pSrc);
	pDev->SetTexture(1, NULL);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	/*D3DRECT rt;
	rt.x1 = pDstRT->left;
	rt.y1 = pDstRT->top;
	rt.x2 = pDstRT->right;
	rt.y2 = pDstRT->bottom;
	pDev->Clear(1, &rt, D3DCLEAR_TARGET, 0xFFFFFFFF, 1.0f, 0);*/

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
	pDev->SetRenderState(D3DRS_CULLMODE, dwCullMode);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlending);
}



VOID D2GIBlitter::BlitWithColorKey(D3D9::IDirect3DSurface9* pDst, RECT* pDstRT,
	D3D9::IDirect3DTexture9* pSrc, RECT* pSrcRT, DWORD dwColorKey)
{
	IDirect3DDevice9* pDev = GetD3D9Device();
	FLOAT afSrcRect[] = { 0.0, 0.0, 1.0f, 1.0f }, afBiasRect[] = {0.0, 0.0, 0.0, 0.0};
	D3DVIEWPORT9 sOriginalVP, sUsedVP;
	DWORD dwZEnable, dwZWriteEnable;
	D3DSURFACE_DESC sDstDesc, sSrcDesc;
	IDirect3DBaseTexture9* pCurrentTexture1 = NULL, * pCurrentTexture2 = NULL;
	DWORD dwMinFilter, dwMagFilter, dwCullMode, dwAlphaBlending;
	FLOAT afColorKey[] = 
	{
		(FLOAT)((dwColorKey >> 24) & 0xFF) / 255.0f,
		(FLOAT)((dwColorKey >> 16) & 0xFF) / 255.0f,
		(FLOAT)((dwColorKey >> 8) & 0xFF) / 255.0f,
		(FLOAT)((dwColorKey) & 0xFF) / 255.0f,
	};
	DWORD        dwAlphaTestEnable, dwAlphaTestFunc, dwAlphaTestRef;

	pDst->GetDesc(&sSrcDesc);
	pSrc->GetLevelDesc(0, &sSrcDesc);

	pDev->GetViewport(&sOriginalVP);
	pDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	pDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWriteEnable);
	pDev->GetTexture(0, &pCurrentTexture1);
	pDev->GetTexture(1, &pCurrentTexture2);
	pDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
	pDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
	pDev->GetRenderState(D3DRS_CULLMODE, &dwCullMode);
	pDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlending);
	pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable);
	pDev->GetRenderState(D3D9::D3DRS_ALPHAFUNC, &dwAlphaTestFunc);
	pDev->GetRenderState(D3D9::D3DRS_ALPHAREF, &dwAlphaTestRef);

	if (pSrcRT != NULL)
	{
		afSrcRect[0] = (FLOAT)pSrcRT->left / (FLOAT)sSrcDesc.Width;
		afSrcRect[1] = (FLOAT)pSrcRT->top / (FLOAT)sSrcDesc.Height;
		afSrcRect[2] = (FLOAT)(pSrcRT->right - pSrcRT->left) / (FLOAT)sSrcDesc.Width;
		afSrcRect[3] = (FLOAT)(pSrcRT->bottom - pSrcRT->top) / (FLOAT)sSrcDesc.Height;
	}

	afBiasRect[0] = 0.5f / (FLOAT)(pDstRT->right - pDstRT->left);
	afBiasRect[1] = 0.5f / (FLOAT)(pDstRT->bottom - pDstRT->top);

	pDev->SetRenderTarget(0, pDst);

	pDev->SetVertexDeclaration(m_pVDecl);
	pDev->SetStreamSource(0, m_pVB, 0, sizeof(FLOAT) * 3);

	pDev->SetVertexShader(m_pVS);
	pDev->SetPixelShader(m_pPS);

	pDev->SetPixelShaderConstantF(0, afSrcRect, 1);
	pDev->SetPixelShaderConstantF(1, afBiasRect, 1);
	//pDev->SetPixelShaderConstantF(2, afColorKey, 1);

	pDev->SetTexture(0, pSrc);
	pDev->SetTexture(1, NULL);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

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
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, TRUE);
	pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, D3D9::D3DCMP_GREATEREQUAL);
	pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, 0x00000080);

	pDev->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	/*D3DRECT rt;
	rt.x1 = pDstRT->left;
	rt.y1 = pDstRT->top;
	rt.x2 = pDstRT->right;
	rt.y2 = pDstRT->bottom;
	pDev->Clear(1, &rt, D3DCLEAR_TARGET, 0xFFFFFFFF, 1.0f, 0);*/

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
	pDev->SetRenderState(D3DRS_CULLMODE, dwCullMode);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlending);
	pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwAlphaTestEnable);
	pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, dwAlphaTestFunc);
	pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, dwAlphaTestRef);
}
