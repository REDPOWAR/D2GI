
#include "../common/common.h"
#include "../common/frect.h"
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_blitter.h"

#include "d2gi_blitter_vs.h"
#include "d2gi_blitter_ps.h"


using namespace D3D9;


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
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	IDirect3DDevice9* pDev = GetD3D9Device();

	if (FAILED(pDev->CreateVertexDeclaration(asVertexElements, &m_pVDecl)))
		Logger::Error(TEXT("Failed to create blitter vertex declaration"));

	FLOAT afVerts[] =
	{
		-0.5f, 0.5f, 0.0,  0.0, 0.0,
		0.5f, 0.5f, 0.0,  1.0f, 0.0,
		-0.5f, -0.5f, 0.0,  0.0, 1.0f,

		0.5f, 0.5f, 0.0,  1.0f, 0.0,
		0.5f, -0.5f, 0.0,  1.0f, 1.0f,
		-0.5f, -0.5f, 0.0, 0.0, 1.0f,
	};
	VOID* pData;

	if (FAILED(pDev->CreateVertexBuffer(sizeof(afVerts), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_pVB, NULL)))
		Logger::Error(TEXT("Failed to create blitter vertex buffer"));

	if (FAILED(m_pVB->Lock(0, 0, &pData, 0)))
		Logger::Error(TEXT("Failed to lock blitter vertex buffer"));

	CopyMemory(pData, afVerts, sizeof(afVerts));
	m_pVB->Unlock();


	if (FAILED(pDev->CreateVertexShader((DWORD*)g_pBlitterVS, &m_pVS)))
		Logger::Error(TEXT("Failed to create blitter vertex shader"));

	if (FAILED(pDev->CreatePixelShader((DWORD*)g_pBlitterPS, &m_pPS)))
		Logger::Error(TEXT("Failed to create blitter pixel shader"));
}


VOID D2GIBlitter::Blit(IDirect3DSurface9* pDst, FRECT* pDstRT,
	IDirect3DTexture9* pSrc, FRECT* pSrcRT, BOOL bEmulateCK)
{
	IDirect3DDevice9* pDev = GetD3D9Device();

	D3DXVECTOR4 vTextureRect, vScreenPosRect, vTextureBorder;

	D3DSURFACE_DESC sDstDesc, sSrcDesc;
	FRECT rtSrc, rtDst;

	IDirect3DSurface9* pOriginalRT = NULL, *pOriginalDS;
	IDirect3DBaseTexture9* pCurrentTexture1 = NULL, * pCurrentTexture2 = NULL;
	DWORD dwMinFilter, dwMagFilter, dwCullMode, dwAlphaTestEnable; 
	DWORD dwAlphaBlending, dwAlphaOp, dwAlphaSrc, dwAlphaDst;
	DWORD dwZEnable, dwZWriteEnable;
	D3DVIEWPORT9 sOriginalVP, sUsedVP;

	pDst->GetDesc(&sDstDesc);
	pSrc->GetLevelDesc(0, &sSrcDesc);

	if (pSrcRT != NULL)
		rtSrc = *pSrcRT;
	else
		rtSrc = FRECT(0.0, 0.0, (FLOAT)sSrcDesc.Width, (FLOAT)sSrcDesc.Height);

	if (pDstRT != NULL)
		rtDst = *pDstRT;
	else
		rtDst = FRECT(0.0, 0.0, (FLOAT)sDstDesc.Width, (FLOAT)sDstDesc.Height);

	pDev->GetRenderTarget(0, &pOriginalRT);
	pDev->GetDepthStencilSurface(&pOriginalDS);
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
	if (bEmulateCK)
	{
		pDev->GetRenderState(D3D9::D3DRS_BLENDOP, &dwAlphaOp);
		pDev->GetRenderState(D3D9::D3DRS_SRCBLEND, &dwAlphaSrc);
		pDev->GetRenderState(D3D9::D3DRS_DESTBLEND, &dwAlphaDst);
	}

	vTextureRect.x = rtSrc.GetWidth() / (FLOAT)sSrcDesc.Width;
	vTextureRect.y = rtSrc.GetHeight() / (FLOAT)sSrcDesc.Height;
	vTextureRect.z = rtSrc.fLeft / (FLOAT)sSrcDesc.Width;
	vTextureRect.w = rtSrc.fTop / (FLOAT)sSrcDesc.Height;

	vTextureBorder.x = (rtSrc.fLeft + 0.5f) / sSrcDesc.Width;
	vTextureBorder.y = (rtSrc.fTop + 0.5f) / sSrcDesc.Height;
	vTextureBorder.z = (rtSrc.fRight - 0.5f) / sSrcDesc.Width;
	vTextureBorder.w = (rtSrc.fBottom - 0.5f) / sSrcDesc.Height;

	vScreenPosRect.x = 2.0f * rtDst.GetWidth() / (FLOAT)sDstDesc.Width;
	vScreenPosRect.y = 2.0f * rtDst.GetHeight() / (FLOAT)sDstDesc.Height;
	vScreenPosRect.z = 2.0f * (-0.5f + rtDst.GetXCenter()) / (FLOAT)sDstDesc.Width - 1.0f;
	vScreenPosRect.w = -2.0f * (-0.5f + rtDst.GetYCenter()) / (FLOAT)sDstDesc.Height + 1.0f;

	sUsedVP.X = 0;
	sUsedVP.Y = 0;
	sUsedVP.Width = sDstDesc.Width;
	sUsedVP.Height = sDstDesc.Height;
	sUsedVP.MinZ = 0.0;
	sUsedVP.MaxZ = 1.0f;

	if(pDst != pOriginalRT)
		pDev->SetRenderTarget(0, pDst);
	pDev->SetDepthStencilSurface(NULL);
	pDev->SetViewport(&sUsedVP);

	pDev->SetVertexDeclaration(m_pVDecl);
	pDev->SetStreamSource(0, m_pVB, 0, sizeof(FLOAT) * 5);

	pDev->SetVertexShader(m_pVS);
	pDev->SetVertexShaderConstantF(0, &vScreenPosRect.x, 1);
	pDev->SetVertexShaderConstantF(1, &vTextureRect.x, 1);

	pDev->SetPixelShader(m_pPS);
	pDev->SetPixelShaderConstantF(0, &vTextureBorder.x, 1);


	pDev->SetTexture(0, pSrc);
	pDev->SetTexture(1, NULL);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	if (bEmulateCK)
	{
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}else
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

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
	pDev->SetRenderState(D3DRS_CULLMODE, dwCullMode);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlending);
	pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwAlphaTestEnable);
	if (bEmulateCK)
	{
		pDev->SetRenderState(D3DRS_BLENDOP, dwAlphaOp);
		pDev->SetRenderState(D3DRS_SRCBLEND, dwAlphaSrc);
		pDev->SetRenderState(D3DRS_DESTBLEND, dwAlphaDst);
	}
	if (pDst != pOriginalRT)
		pDev->SetRenderTarget(0, pOriginalRT);
	pDev->SetDepthStencilSurface(pOriginalDS);
	RELEASE(pOriginalDS);
	RELEASE(pOriginalRT);
}
