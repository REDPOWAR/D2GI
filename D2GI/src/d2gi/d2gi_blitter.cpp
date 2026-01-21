
#include "../common/common.h"
#include "../common/frect.h"
#include "../common/logger.h"

#include "d2gi.h"
#include "d2gi_blitter.h"

#include "d2gi_blitter_vs.h"
#include "d2gi_blitter_ps.h"


using namespace D3D9;

void D2GIBlitter::ReleaseResource()
{
	m_pVB.Reset();
	m_pVDecl.Reset();
	m_pVS.Reset();
	m_pPS.Reset();
}


void D2GIBlitter::LoadResource()
{
	const D3DVERTEXELEMENT9 asVertexElements[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 8, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	IDirect3DDevice9* pDev = GetD3D9Device();

	if (FAILED(pDev->CreateVertexDeclaration(asVertexElements, &m_pVDecl)))
		Logger::Error(TEXT("Failed to create blitter vertex declaration"));

	const FLOAT afVerts[] =
	{
		-0.5f, 0.5f,  0.0, 0.0,
		0.5f, 0.5f,  1.0f, 0.0,
		-0.5f, -0.5f,  0.0, 1.0f,

		0.5f, -0.5f,  1.0f, 1.0f,
	};
	void* pData;

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

void D2GIBlitter::Blit(IDirect3DSurface9* pDst, const FRECT* pDstRT,
	IDirect3DTexture9* pSrc, const FRECT* pSrcRT, bool bEmulateCK)
{
	IDirect3DDevice9* pDev = GetD3D9Device();

	struct
	{
		float x, y, z, w;
	} vsConstants[2], psConstants[1];

	auto& vScreenPosRect = vsConstants[0];
	auto& vTextureRect = vsConstants[1];
	auto& vTextureBorder = psConstants[0];

	D3DSURFACE_DESC sDstDesc, sSrcDesc;
	FRECT rtSrc, rtDst;

	Microsoft::WRL::ComPtr<IDirect3DSurface9> pOriginalRT;
	Microsoft::WRL::ComPtr<IDirect3DBaseTexture9> pCurrentTexture;

	DWORD dwMinFilter, dwMagFilter, dwCullMode, dwAlphaTestEnable;
	DWORD dwAlphaBlending, dwAlphaOp, dwAlphaSrc, dwAlphaDst;
	DWORD dwZEnable, dwZWriteEnable;
	DWORD dwFogEnable;
	D3DVIEWPORT9 sOriginalVP, sUsedVP;

	pDst->GetDesc(&sDstDesc);
	pSrc->GetLevelDesc(0, &sSrcDesc);

	if (pSrcRT != nullptr)
		rtSrc = *pSrcRT;
	else
		rtSrc = FRECT(0.0, 0.0, (FLOAT)sSrcDesc.Width, (FLOAT)sSrcDesc.Height);

	if (pDstRT != nullptr)
		rtDst = *pDstRT;
	else
		rtDst = FRECT(0.0, 0.0, (FLOAT)sDstDesc.Width, (FLOAT)sDstDesc.Height);

	pDev->GetRenderTarget(0, pOriginalRT.GetAddressOf());
	pDev->GetViewport(&sOriginalVP);
	pDev->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	pDev->GetRenderState(D3DRS_ZWRITEENABLE, &dwZWriteEnable);
	pDev->GetTexture(0, pCurrentTexture.GetAddressOf());
	pDev->GetSamplerState(0, D3DSAMP_MINFILTER, &dwMinFilter);
	pDev->GetSamplerState(0, D3DSAMP_MAGFILTER, &dwMagFilter);
	pDev->GetRenderState(D3DRS_CULLMODE, &dwCullMode);
	pDev->GetRenderState(D3DRS_ALPHABLENDENABLE, &dwAlphaBlending);
	pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable);
	pDev->GetRenderState(D3D9::D3DRS_FOGENABLE, &dwFogEnable);
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

	// Calculate the rectangle position in clip space [-1, 1],
	// shifted by half the viewport unit to account for the half-pixel offset.
	// 2.0 * (rtDst.Center() - 0.5) / sDstDesc + 1.0
	vScreenPosRect.z = (-1.0f + rtDst.fLeft + rtDst.fRight) / (FLOAT)sDstDesc.Width - 1.0f;
	vScreenPosRect.w = -(-1.0f + rtDst.fTop + rtDst.fBottom) / (FLOAT)sDstDesc.Height + 1.0f;

	sUsedVP.X = 0;
	sUsedVP.Y = 0;
	sUsedVP.Width = sDstDesc.Width;
	sUsedVP.Height = sDstDesc.Height;
	sUsedVP.MinZ = 0.0;
	sUsedVP.MaxZ = 1.0f;

	if (pDst != pOriginalRT.Get())
		pDev->SetRenderTarget(0, pDst);
	pDev->SetViewport(&sUsedVP);

	pDev->SetVertexDeclaration(m_pVDecl.Get());
	pDev->SetStreamSource(0, m_pVB.Get(), 0, sizeof(FLOAT) * 4);

	pDev->SetVertexShader(m_pVS.Get());
	pDev->SetVertexShaderConstantF(0, reinterpret_cast<float*>(vsConstants), std::size(vsConstants));

	pDev->SetPixelShader(m_pPS.Get());
	pDev->SetPixelShaderConstantF(0, reinterpret_cast<float*>(psConstants), std::size(psConstants));

	pDev->SetTexture(0, pSrc);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pDev->SetRenderState(D3DRS_ZENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	pDev->SetRenderState(D3DRS_FOGENABLE, FALSE);
	if (bEmulateCK)
	{
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		pDev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
		pDev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		pDev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	else
		pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	pDev->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	pDev->SetPixelShader(NULL);
	pDev->SetVertexShader(NULL);
	pDev->SetTexture(0, pCurrentTexture.Get());
	pDev->SetViewport(&sOriginalVP);
	pDev->SetSamplerState(0, D3DSAMP_MINFILTER, dwMinFilter);
	pDev->SetSamplerState(0, D3DSAMP_MAGFILTER, dwMagFilter);
	pDev->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	pDev->SetRenderState(D3DRS_ZWRITEENABLE, dwZWriteEnable);
	pDev->SetRenderState(D3DRS_CULLMODE, dwCullMode);
	pDev->SetRenderState(D3DRS_ALPHABLENDENABLE, dwAlphaBlending);
	pDev->SetRenderState(D3DRS_ALPHATESTENABLE, dwAlphaTestEnable);
	pDev->SetRenderState(D3DRS_FOGENABLE, dwFogEnable);
	if (bEmulateCK)
	{
		pDev->SetRenderState(D3DRS_BLENDOP, dwAlphaOp);
		pDev->SetRenderState(D3DRS_SRCBLEND, dwAlphaSrc);
		pDev->SetRenderState(D3DRS_DESTBLEND, dwAlphaDst);
	}
	if (pDst != pOriginalRT.Get())
		pDev->SetRenderTarget(0, pOriginalRT.Get());
}