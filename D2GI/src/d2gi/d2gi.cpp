
#include "../common.h"
#include "../utils.h"
#include "../hooks.h"
#include "../m3x4.h"
#include "../frect.h"
#include "../logger.h"
#include "../dir.h"

#include "d2gi.h"
#include "d2gi_ddraw.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_backbuf_surf.h"
#include "d2gi_blitter.h"
#include "d2gi_prim_single_surf.h"
#include "d2gi_sysmem_surf.h"
#include "d2gi_texture.h"
#include "d2gi_strided_renderer.h"


D2GI::D2GI()
{
	m_hD3D9Lib = NULL;
	m_pD3D9 = NULL;
	m_pDev = NULL;
	m_pBackBufferCopy = NULL;
	m_pBackBufferCopySurf = NULL;
	m_pfnOriginalWndProc = NULL;

	m_eRenderState = RS_UNKNOWN;
	m_bSceneBegun = FALSE;
	m_bColorKeyEnabled = FALSE;

	m_dwForcedWidth = 1366;
	m_dwForcedHeight = 768;

	ZeroMemory(m_lpCurrentTextures, sizeof(m_lpCurrentTextures));

	m_pClearRects = new D3D9RECTVector();
	m_p2DBuffer = new ByteBuffer();

	m_pDirectDrawProxy = new D2GIDirectDraw(this);
	m_pBlitter = new D2GIBlitter(this);
	m_pStridedRenderer = new D2GIStridedPrimitiveRenderer(this);

	LoadD3D9Library();
}


D2GI::~D2GI()
{
	DetachWndProc();
	DEL(m_pStridedRenderer);
	DEL(m_pBlitter);
	DEL(m_pClearRects);
	RELEASE(m_pDev);
	RELEASE(m_pD3D9);
	FreeLibrary(m_hD3D9Lib);
}


VOID D2GI::OnDirectDrawReleased()
{
	delete this;
}


VOID D2GI::LoadD3D9Library()
{
	typedef D3D9::IDirect3D9* (WINAPI* DIRECT3DCREATE9)(UINT);

	TCHAR           szPath[MAX_PATH];
	DIRECT3DCREATE9 pfnDirect3DCreate9;

	_tcscpy(szPath, Directory::GetSysDirectory());
	_tcscat(szPath, TEXT("\\d3d9.dll"));

	m_hD3D9Lib = LoadLibrary(szPath);
	if (m_hD3D9Lib == NULL)
		Logger::Error(TEXT("Failed to load D3D9 library"));

	pfnDirect3DCreate9 = (DIRECT3DCREATE9)GetProcAddress(m_hD3D9Lib, "Direct3DCreate9");
	if (pfnDirect3DCreate9 == NULL)
		Logger::Error(TEXT("Failed to get Direct3DCreate9 address"));

	m_pD3D9 = pfnDirect3DCreate9(D3D_SDK_VERSION);
	if (m_pD3D9 == NULL)
		Logger::Error(TEXT("Failed to obtain IDirect3D9 interface"));
}


VOID D2GI::OnCooperativeLevelSet(HWND hWnd, DWORD dwFlags)
{
	m_hWnd = hWnd;
	Logger::SetHWND(hWnd);
	AttachWndProc();
}


VOID D2GI::OnDisplayModeSet(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwFlags)
{
	m_dwOriginalWidth = dwWidth;
	m_dwOriginalHeight = dwHeight;
	m_dwOriginalBPP = dwBPP;

	m_fWidthScale = (FLOAT)m_dwForcedWidth / (FLOAT)m_dwOriginalWidth;
	m_fHeightScale = (FLOAT)m_dwForcedHeight / (FLOAT)m_dwOriginalHeight;
	m_fAspectRatioScale = ((FLOAT)m_dwForcedWidth / (FLOAT)m_dwForcedHeight);
	m_fAspectRatioScale /= ((FLOAT)m_dwOriginalWidth / (FLOAT)m_dwOriginalHeight);

	ResetD3D9Device();
}


VOID D2GI::ReleaseResources()
{
	m_pDirectDrawProxy->ReleaseResources();
	m_pBlitter->ReleaseResource();
	m_pStridedRenderer->ReleaseResource();
}


VOID D2GI::LoadResources()
{
	m_pDirectDrawProxy->LoadResources();
	m_pBlitter->LoadResource();
	m_pStridedRenderer->LoadResource();
}


VOID D2GI::ResetD3D9Device()
{
	D3D9::D3DPRESENT_PARAMETERS sParams;

	ReleaseResources();
	RELEASE(m_pBackBufferCopySurf);
	RELEASE(m_pBackBufferCopy);
	RELEASE(m_pDev);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, 0);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_dwForcedWidth, m_dwForcedHeight, SWP_DRAWFRAME);

	ZeroMemory(&sParams, sizeof(sParams));
	sParams.AutoDepthStencilFormat = D3D9::D3DFMT_D24X8;
	sParams.EnableAutoDepthStencil = TRUE;
	sParams.BackBufferCount = 1;
	sParams.BackBufferWidth = m_dwForcedWidth;
	sParams.BackBufferHeight = m_dwForcedHeight;
	sParams.BackBufferFormat = D3D9::D3DFMT_A8R8G8B8;
	sParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	sParams.SwapEffect = D3D9::D3DSWAPEFFECT_FLIP;
	sParams.Windowed = TRUE;
	//sParams.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

	if (FAILED(m_pD3D9->CreateDevice(0, D3D9::D3DDEVTYPE_HAL, m_hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED | D3DCREATE_FPU_PRESERVE,
		&sParams, &m_pDev)))
		Logger::Error(TEXT("Failed to create D3D9 device"));

	if (FAILED(m_pDev->CreateTexture(m_dwForcedWidth, m_dwOriginalHeight, 1, D3DUSAGE_RENDERTARGET,
		D3D9::D3DFMT_A8R8G8B8, D3D9::D3DPOOL_DEFAULT, &m_pBackBufferCopy, NULL)))
		Logger::Error(TEXT("Failed to create backbuffer copy texture"));

	if (FAILED(m_pBackBufferCopy->GetSurfaceLevel(0, &m_pBackBufferCopySurf)))
		Logger::Error(TEXT("Failed to get backbuffer copy surface"));

	LoadResources();
}


VOID D2GI::OnViewportSet(D3D7::LPD3DVIEWPORT7 pVP)
{
	D3D9::D3DVIEWPORT9 sD3D9Viewport;
	FRECT frtVP, frtScaledVP;

	frtVP = FRECT(pVP->dwX, pVP->dwY, pVP->dwX + pVP->dwWidth, pVP->dwY + pVP->dwHeight);
	ScaleFRect(&frtVP, &frtScaledVP);

	if (pVP->dwX != 0 && pVP->dwY != 0 
		&& pVP->dwWidth != m_dwOriginalWidth && pVP->dwHeight != m_dwOriginalHeight)
	{
		frtScaledVP.fLeft = max(0.0, floorf(frtScaledVP.fLeft - 0.5f));
		frtScaledVP.fTop = max(0.0, floorf(frtScaledVP.fTop - 0.5f));
		frtScaledVP.fRight = min((FLOAT)m_dwForcedWidth, ceilf(frtScaledVP.fRight + 0.5f));
		frtScaledVP.fBottom = min((FLOAT)m_dwForcedWidth, ceilf(frtScaledVP.fBottom + 0.5f));
	}

	sD3D9Viewport.X = (DWORD)frtScaledVP.fLeft;
	sD3D9Viewport.Y = (DWORD)frtScaledVP.fTop;
	sD3D9Viewport.Width = (DWORD)(frtScaledVP.GetWidth());
	sD3D9Viewport.Height = (DWORD)(frtScaledVP.GetHeight());
	sD3D9Viewport.MinZ = pVP->dvMinZ;
	sD3D9Viewport.MaxZ = pVP->dvMaxZ;
	m_pDev->SetViewport(&sD3D9Viewport);
}


VOID D2GI::OnBackBufferLock(BOOL bRead)
{
	if (!bRead)
	{
		m_eRenderState = RS_BACKBUFFER_STREAMING;
	}
	else
	{
		D3D9::IDirect3DSurface9* pRT;
		D2GIBackBufferSurface* pBackBuf;


		if (!m_bSceneBegun)
			m_pDev->BeginScene();

		pBackBuf = m_pDirectDrawProxy->GetPrimaryFlippableSurface()->GetBackBufferSurface();

		m_pDev->GetRenderTarget(0, &pRT);
		m_pDev->StretchRect(pRT, NULL, m_pBackBufferCopySurf, NULL, D3D9::D3DTEXF_LINEAR);
		m_pBlitter->Blit(pBackBuf->GetD3D9ReadingSurface(), NULL, m_pBackBufferCopy, NULL, FALSE);
		pRT->Release();

		if (!m_bSceneBegun)
			m_pDev->EndScene();
	}
}


VOID D2GI::OnFlip()
{
	if (m_eRenderState == RS_BACKBUFFER_STREAMING)
	{
		D2GIPrimaryFlippableSurface* pPrimSurf = m_pDirectDrawProxy->GetPrimaryFlippableSurface();
		D3D9::IDirect3DSurface9* pSurf = pPrimSurf->GetBackBufferSurface()->GetD3D9StreamingSurface();
		D3D9::IDirect3DSurface9* pRT;

		m_pDev->GetRenderTarget(0, &pRT);
		m_pDev->StretchRect(pSurf, NULL, pRT, NULL, D3D9::D3DTEXF_POINT);
		Present();

		pRT->Release();
	}
	else if (m_eRenderState == RS_BACKBUFFER_BLITTING || m_eRenderState == RS_3D_RENDERING)
	{
		Present();
	}
}


VOID D2GI::OnSysMemSurfaceBltOnPrimarySingle(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GIPrimarySingleSurface* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	m_eRenderState = RS_PRIMARY_SURFACE_BLITTING;

	if (m_dwOriginalBPP == 8)
	{
		RECT sScaledRect;

		ScaleRect(pDstRT, &sScaledRect);

		pSrc->UpdateWithPalette(pDst->GetPalette());
		m_pDev->GetRenderTarget(0, &pRT);
		m_pDev->StretchRect(pSrc->GetD3D9Surface(), pSrcRT, pRT, &sScaledRect, D3D9::D3DTEXF_LINEAR);
		Present();

		pRT->Release();
	}
}


VOID D2GI::OnClear(DWORD dwCount, D3D7::LPD3DRECT pRects, DWORD dwFlags, D3D7::D3DCOLOR col, D3D7::D3DVALUE z, DWORD dwStencil)
{
	INT i;

	m_pClearRects->clear();

	for (i = 0; i < dwCount; i++)
	{
		D3D9::D3DRECT sRect;

		ScaleD3D9Rect((D3D9::D3DRECT*)pRects + i, &sRect);
		m_pClearRects->push_back(sRect);
	}

	m_pDev->Clear(dwCount, m_pClearRects->data(), dwFlags, col, z, dwStencil);
}


VOID D2GI::OnLightEnable(DWORD i, BOOL bEnable)
{
	m_pDev->LightEnable(i, bEnable);
}


VOID D2GI::OnSysMemSurfaceBltOnBackBuffer(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GIBackBufferSurface* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT = NULL;
	D3D9::D3DSURFACE_DESC sSrcDesc, sDstDesc;
	FRECT frtSrc, frtDst;
	FRECT frtScaledDst;

	m_eRenderState = RS_BACKBUFFER_BLITTING;


	//BeginScene();
	m_pDev->GetRenderTarget(0, &pRT);
	if(!m_bSceneBegun)
		m_pDev->BeginScene();


	pSrc->GetD3D9Texture()->GetLevelDesc(0, &sSrcDesc);
	pRT->GetDesc(&sDstDesc);
	if (pSrcRT != NULL)
		frtSrc = FRECT(*pSrcRT);
	else
		frtSrc = FRECT(0, 0, sSrcDesc.Width, sSrcDesc.Height);

	if (pDstRT != NULL)
		frtDst = FRECT(*pDstRT);
	else
		frtDst = FRECT(0, 0, sDstDesc.Width, sDstDesc.Height);

	

	ScaleFRect(&frtDst, &frtScaledDst);
	m_pBlitter->Blit(pRT, &frtScaledDst,
		pSrc->GetD3D9Texture(), &frtSrc, pSrc->HasColorKeyConversion());

	if (!m_bSceneBegun)
		m_pDev->EndScene();

	pRT->Release();
	//EndScene();

}


VOID D2GI::OnSysMemSurfaceBltOnTexture(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GITexture* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	// MULTITHREADED_ACCESS

	if (pDst->GetWidth() != pSrc->GetWidth() || pDst->GetHeight() != pSrc->GetHeight())
		return;

	VOID* pData;
	D3D9::D3DLOCKED_RECT rt;
	INT i, j;

	pDst->GetD3D9Texture()->LockRect(0, &rt, NULL, D3DLOCK_DISCARD);

	for (i = 0; i < (INT)pDst->GetHeight(); i++)
		CopyMemory((BYTE*)rt.pBits + i * rt.Pitch, 
		(BYTE*)pSrc->GetData() + i * pSrc->GetDataPitch(), pSrc->GetDataPitch());

	pDst->GetD3D9Texture()->UnlockRect(0);
}


VOID D2GI::OnSceneBegin()
{
	m_eRenderState = RS_3D_RENDERING;
	BeginScene();
}


VOID D2GI::BeginScene()
{
	if (!m_bSceneBegun)
	{
		m_pDev->BeginScene();
		m_bSceneBegun = TRUE;
	}
}


VOID D2GI::OnSceneEnd()
{
	m_eRenderState = RS_3D_RENDERING;
	EndScene();
}


VOID D2GI::EndScene()
{
	if (m_bSceneBegun)
	{
		m_pDev->EndScene();
		m_bSceneBegun = FALSE;
	}
}


VOID D2GI::OnRenderStateSet(D3D7::D3DRENDERSTATETYPE eState, DWORD dwValue)
{
	switch (eState)
	{
		case D3D7::D3DRENDERSTATE_CULLMODE:
			m_pDev->SetRenderState(D3D9::D3DRS_CULLMODE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_TEXTUREPERSPECTIVE:
			break;
		case D3D7::D3DRENDERSTATE_COLORKEYENABLE:
			m_bColorKeyEnabled = dwValue;
			break;
		case D3D7::D3DRENDERSTATE_ZENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_ZENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_SHADEMODE:
			m_pDev->SetRenderState(D3D9::D3DRS_SHADEMODE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_DITHERENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_DITHERENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_SRCBLEND:
			m_pDev->SetRenderState(D3D9::D3DRS_SRCBLEND, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_DESTBLEND:
			m_pDev->SetRenderState(D3D9::D3DRS_DESTBLEND, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_ALPHABLENDENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_ALPHABLENDENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_SPECULARENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_SPECULARENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_COLORVERTEX:
			m_pDev->SetRenderState(D3D9::D3DRS_COLORVERTEX, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_AMBIENTMATERIALSOURCE:
			m_pDev->SetRenderState(D3D9::D3DRS_AMBIENTMATERIALSOURCE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_SPECULARMATERIALSOURCE:
			m_pDev->SetRenderState(D3D9::D3DRS_SPECULARMATERIALSOURCE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_EMISSIVEMATERIALSOURCE:
			m_pDev->SetRenderState(D3D9::D3DRS_EMISSIVEMATERIALSOURCE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_DIFFUSEMATERIALSOURCE:
			m_pDev->SetRenderState(D3D9::D3DRS_DIFFUSEMATERIALSOURCE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_ALPHATESTENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_FOGENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_FOGENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_FOGCOLOR:
			m_pDev->SetRenderState(D3D9::D3DRS_FOGCOLOR, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_FOGVERTEXMODE:
			m_pDev->SetRenderState(D3D9::D3DRS_FOGVERTEXMODE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_FOGSTART:
			m_pDev->SetRenderState(D3D9::D3DRS_FOGSTART, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_FOGEND:
			m_pDev->SetRenderState(D3D9::D3DRS_FOGEND, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_AMBIENT:
			m_pDev->SetRenderState(D3D9::D3DRS_AMBIENT, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_ZWRITEENABLE:
			m_pDev->SetRenderState(D3D9::D3DRS_ZWRITEENABLE, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_ZFUNC:
			m_pDev->SetRenderState(D3D9::D3DRS_ZFUNC, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_CLIPPING:
			m_pDev->SetRenderState(D3D9::D3DRS_CLIPPING, dwValue);
			break;
		case D3D7::D3DRENDERSTATE_TEXTUREFACTOR:
			m_pDev->SetRenderState(D3D9::D3DRS_TEXTUREFACTOR, dwValue);
			break;
	}
}


VOID D2GI::OnTextureStageSet(DWORD i, D3D7::D3DTEXTURESTAGESTATETYPE eState, DWORD dwValue)
{
	D3D9::D3DTEXTUREFILTERTYPE aeMagTexFMap[] =
	{
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_POINT,
		D3D9::D3DTEXF_LINEAR,
		D3D9::D3DTEXF_PYRAMIDALQUAD,
		D3D9::D3DTEXF_GAUSSIANQUAD,
		D3D9::D3DTEXF_ANISOTROPIC,
	};

	D3D9::D3DTEXTUREFILTERTYPE aeMinTexFMap[] =
	{
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_POINT,
		D3D9::D3DTEXF_LINEAR,
		D3D9::D3DTEXF_ANISOTROPIC,
	};

	D3D9::D3DTEXTUREFILTERTYPE aeMipTexFMap[] =
	{
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_NONE,
		D3D9::D3DTEXF_POINT,
		D3D9::D3DTEXF_LINEAR,
	};

	switch (eState)
	{
		case D3D7::D3DTSS_COLOROP:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_COLOROP, dwValue);
			break;
		case D3D7::D3DTSS_COLORARG1:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_COLORARG1, dwValue);
			break;
		case D3D7::D3DTSS_COLORARG2:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_COLORARG2, dwValue);
			break;
		case D3D7::D3DTSS_ALPHAOP:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_ALPHAOP, dwValue);
			break;
		case D3D7::D3DTSS_ALPHAARG1:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_ALPHAARG1, dwValue);
			break;
		case D3D7::D3DTSS_ALPHAARG2:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_ALPHAARG2, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT00:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT00, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT01:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT01, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT10:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT10, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVMAT11:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVMAT11, dwValue);
			break;
		case D3D7::D3DTSS_TEXCOORDINDEX:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_TEXCOORDINDEX, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVLSCALE:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVLSCALE, dwValue);
			break;
		case D3D7::D3DTSS_BUMPENVLOFFSET:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_BUMPENVLOFFSET, dwValue);
			break;
		case D3D7::D3DTSS_TEXTURETRANSFORMFLAGS:
			m_pDev->SetTextureStageState(i, D3D9::D3DTSS_TEXTURETRANSFORMFLAGS, dwValue);
			break;

		case D3D7::D3DTSS_ADDRESS:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSU, dwValue);
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSV, dwValue);
			break;
		case D3D7::D3DTSS_ADDRESSU:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSU, dwValue);
			break;
		case D3D7::D3DTSS_ADDRESSV:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_ADDRESSV, dwValue);
			break;
		case D3D7::D3DTSS_BORDERCOLOR:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_BORDERCOLOR, dwValue);
			break;
		case D3D7::D3DTSS_MAGFILTER:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MAGFILTER, aeMagTexFMap[dwValue]);
			break;
		case D3D7::D3DTSS_MINFILTER:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MINFILTER, aeMinTexFMap[dwValue]);
			break;
		case D3D7::D3DTSS_MIPFILTER:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MIPFILTER, aeMipTexFMap[dwValue]);
			break;
		case D3D7::D3DTSS_MIPMAPLODBIAS:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MIPMAPLODBIAS, dwValue);
			break;
		case D3D7::D3DTSS_MAXMIPLEVEL:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MAXMIPLEVEL, dwValue);
			break;
		case D3D7::D3DTSS_MAXANISOTROPY:
			m_pDev->SetSamplerState(i, D3D9::D3DSAMP_MAXANISOTROPY, dwValue);
			break;
	}

}


VOID D2GI::OnTextureSet(DWORD i, D2GITexture* pTex)
{
	m_lpCurrentTextures[i] = pTex;

	m_pDev->SetTexture(i, pTex == NULL ? NULL : pTex->GetD3D9Texture());
}


BOOL D2GI::OnDeviceValidate(DWORD* pdw)
{
	return SUCCEEDED(m_pDev->ValidateDevice(pdw));
}


VOID D2GI::OnTransformSet(D3D7::D3DTRANSFORMSTATETYPE eType, D3D7::D3DMATRIX* pMatrix)
{
	switch (eType)
	{
		case D3D7::D3DTRANSFORMSTATE_WORLD:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD, (D3D9::D3DMATRIX*)pMatrix);
			break;
		case D3D7::D3DTRANSFORMSTATE_WORLD1:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD1, (D3D9::D3DMATRIX*)pMatrix);
			break;
		case D3D7::D3DTRANSFORMSTATE_WORLD2:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD2, (D3D9::D3DMATRIX*)pMatrix);
			break;
		case D3D7::D3DTRANSFORMSTATE_WORLD3:
			m_pDev->SetTransform(D3D9::D3DTS_WORLD3, (D3D9::D3DMATRIX*)pMatrix);
			break;
		default:
			m_pDev->SetTransform((D3D9::D3DTRANSFORMSTATETYPE)eType, (D3D9::D3DMATRIX*)pMatrix);
			break;
	}
}


VOID D2GI::OnLightSet(DWORD i, D3D7::LPD3DLIGHT7 pLight)
{
	m_pDev->SetLight(i, (D3D9::D3DLIGHT9*)pLight);
}


VOID D2GI::OnMaterialSet(D3D7::LPD3DMATERIAL7 pMaterial)
{
	m_pDev->SetMaterial((D3D9::D3DMATERIAL9*)pMaterial);
}


VOID D2GI::OnClipStatusSet(D3D7::LPD3DCLIPSTATUS pStatus)
{
	D3D9::D3DCLIPSTATUS9 sStatus9;

	return;

	if (pStatus->dwFlags != D3DCLIPSTATUS_STATUS)
		return;

	if (pStatus->dwStatus &
		~(D3DSTATUS_CLIPUNIONBACK | D3DSTATUS_CLIPUNIONLEFT | D3DSTATUS_CLIPUNIONRIGHT
			| D3DSTATUS_CLIPUNIONBOTTOM | D3DSTATUS_CLIPUNIONTOP))
		return;

	ZeroMemory(&sStatus9, sizeof(sStatus9));
	sStatus9.ClipUnion = 0;
	if (pStatus->dwStatus & D3DSTATUS_CLIPUNIONBACK)
		sStatus9.ClipUnion |= D3DCS_BACK;
	if (pStatus->dwStatus & D3DSTATUS_CLIPUNIONLEFT)
		sStatus9.ClipUnion |= D3DCS_LEFT;
	if (pStatus->dwStatus & D3DSTATUS_CLIPUNIONRIGHT)
		sStatus9.ClipUnion |= D3DCS_RIGHT;
	if (pStatus->dwStatus & D3DSTATUS_CLIPUNIONBOTTOM)
		sStatus9.ClipUnion |= D3DCS_BOTTOM;
	if (pStatus->dwStatus & D3DSTATUS_CLIPUNIONTOP)
		sStatus9.ClipUnion |= D3DCS_TOP;

	m_pDev->SetClipStatus(&sStatus9);
}


VOID D2GI::Present()
{
	m_pDev->Present(NULL, NULL, NULL, NULL);
	m_pStridedRenderer->OnPresentationFinished();
}


VOID D2GI::OnIndexedPrimitiveStridedDraw(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData,
	DWORD dwCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	DrawPrimitive(pt, dwFVF, TRUE, pData, dwCount, pIdx, dwIdxCount, dwFlags);
}


VOID D2GI::OnPrimitiveStridedDraw(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData,
	DWORD dwCount, DWORD dwFlags)
{
	DrawPrimitive(pt, dwFVF, TRUE, pData, dwCount, NULL, 0, dwFlags);
}



VOID D2GI::OnPrimitiveDraw(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, DWORD dwFlags)
{
	if (dwFVF & D3DFVF_XYZRHW)
	{
		UINT uStride = CalcFVFStride(dwFVF);
		UINT uSize = uStride * dwVertCount;
		INT i;

		m_p2DBuffer->clear();
		m_p2DBuffer->insert(m_p2DBuffer->begin(), (BYTE*)pVerts, (BYTE*)pVerts + uSize);
		for (i = 0; i < dwVertCount; i++)
		{
			FLOAT* pV = (FLOAT*)(m_p2DBuffer->data() + i * uStride);
			
			pV[0] *= m_fWidthScale;
			pV[1] *= m_fHeightScale;
		}
		pVerts = m_p2DBuffer->data();
	}

	DrawPrimitive(pt, dwFVF, FALSE, pVerts, dwVertCount, NULL, 0, dwFlags);
}


VOID D2GI::OnIndexedPrimitiveDraw(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	DrawPrimitive(pt, dwFVF, FALSE, pVerts, dwVertCount, pIdx, dwIdxCount, dwFlags);
}


BOOL D2GI::OnRenderStateGet(D3D7::D3DRENDERSTATETYPE eState, DWORD* pValue)
{
	switch (eState)
	{
		case D3D7::D3DRENDERSTATE_CULLMODE:
			m_pDev->GetRenderState(D3D9::D3DRS_CULLMODE, pValue);
			return TRUE;
	}

	return FALSE;
}


VOID D2GI::OnColorFillOnBackBuffer(DWORD dwColor, RECT* pRect)
{
	D3D9::D3DVIEWPORT9 sOriginalViewport, sFillingViewport;


	m_pDev->GetViewport(&sOriginalViewport);
	sFillingViewport.X = pRect->left;
	sFillingViewport.Y = pRect->top;
	sFillingViewport.Width = pRect->right - pRect->left;
	sFillingViewport.Height = pRect->bottom - pRect->top;
	sFillingViewport.MinZ = 0.0;
	sFillingViewport.MaxZ = 1.0f;

	m_pDev->SetViewport(&sFillingViewport);
	m_pDev->Clear(0, NULL, D3DCLEAR_TARGET, dwColor, 1.0f, 0);
	m_pDev->SetViewport(&sOriginalViewport);
}


VOID D2GI::DrawPrimitive(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, BOOL bStrided, VOID* pVertexData,
	DWORD dwVertexCount, WORD* pIndexData, DWORD dwIndexCount, DWORD dwFlags)
{
	DWORD        dwAlphaTestEnable, dwAlphaTestFunc, dwAlphaTestRef;
	D2GITexture* pTexture = m_lpCurrentTextures[0];

	BOOL bEmulateColorKey = (m_bColorKeyEnabled && pTexture != NULL && pTexture->HasColorKeyConversion());


	if (bEmulateColorKey)
	{
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwAlphaTestEnable);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAFUNC, &dwAlphaTestFunc);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAREF, &dwAlphaTestRef);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, TRUE);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, D3D9::D3DCMP_GREATEREQUAL);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, 0x00000080);
	}

	if (bStrided)
	{
		if (pIndexData != NULL)
		{
			m_pStridedRenderer->DrawIndexedPrimitiveStrided(pt, dwFVF,
				(D3D7::D3DDRAWPRIMITIVESTRIDEDDATA*)pVertexData, dwVertexCount,
				pIndexData, dwIndexCount, dwFlags);
		}
		else
		{
			m_pStridedRenderer->DrawPrimitiveStrided(pt, dwFVF,
				(D3D7::D3DDRAWPRIMITIVESTRIDEDDATA*)pVertexData, dwVertexCount, dwFlags);
		}
	}
	else
	{
		UINT uVertexStride, uPrimCount;

		uVertexStride = CalcFVFStride(dwFVF);
		uPrimCount = CalcPrimitiveCount(pt, (pIndexData != NULL) ? dwIndexCount : dwVertexCount);

		m_pDev->SetFVF(dwFVF);
		if (pIndexData != NULL)
		{
			m_pDev->DrawIndexedPrimitiveUP((D3D9::D3DPRIMITIVETYPE)pt, 0, dwVertexCount,
				uPrimCount, pIndexData, D3D9::D3DFMT_INDEX16, pVertexData, uVertexStride);
		}
		else
		{
			// TODO: think about this scene begin/end fix
			if (!m_bSceneBegun)
				m_pDev->BeginScene();
			m_pDev->DrawPrimitiveUP((D3D9::D3DPRIMITIVETYPE)pt, uPrimCount, pVertexData, uVertexStride);
			if (!m_bSceneBegun)
				m_pDev->EndScene();
		}
	}

	if (bEmulateColorKey)
	{
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwAlphaTestEnable);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, dwAlphaTestFunc);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, dwAlphaTestRef);
	}
}


VOID D2GI::ScaleFRect(FRECT* pSrc, FRECT* pOut)
{
	if (pSrc == NULL)
	{
		pOut->fLeft = pOut->fTop = 0;
		pOut->fRight = m_dwForcedWidth;
		pOut->fBottom = m_dwForcedHeight;
		return;
	}

	pOut->fLeft = (pSrc->fLeft * m_fWidthScale);
	pOut->fTop = (pSrc->fTop * m_fHeightScale);
	pOut->fRight = (pSrc->fRight * m_fWidthScale);
	pOut->fBottom = (pSrc->fBottom * m_fHeightScale);
}


VOID D2GI::ScaleRect(RECT* pSrc, RECT* pOut)
{
	if (pSrc == NULL)
	{
		pOut->left = pOut->top = 0;
		pOut->right = m_dwForcedWidth;
		pOut->bottom = m_dwForcedHeight;
		return;
	}

	pOut->left = pSrc->left * m_dwForcedWidth / m_dwOriginalWidth;
	pOut->top = pSrc->top * m_dwForcedHeight / m_dwOriginalHeight;
	pOut->right = pSrc->right * m_dwForcedWidth / m_dwOriginalWidth;
	pOut->bottom = pSrc->bottom * m_dwForcedHeight / m_dwOriginalHeight;
}


VOID D2GI::AttachWndProc()
{
	if (m_pfnOriginalWndProc != NULL)
		return;

	m_pfnOriginalWndProc = (WNDPROC)GetWindowLong(m_hWnd, GWL_WNDPROC);
	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)WndProc_Static);
	SetWindowLong(m_hWnd, GWL_USERDATA, (LONG)this);
}


VOID D2GI::DetachWndProc()
{
	if (m_pfnOriginalWndProc == NULL)
		return;

	SetWindowLong(m_hWnd, GWL_WNDPROC, (LONG)m_pfnOriginalWndProc);
	SetWindowLong(m_hWnd, GWL_USERDATA, 0);
	m_pfnOriginalWndProc = NULL;
}


LRESULT D2GI::WndProc_Static(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return ((D2GI*)GetWindowLong(hWnd, GWL_USERDATA))->WndProc(hWnd, uMsg, wParam, lParam);
}


LRESULT D2GI::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	INT x, y;

	switch (uMsg)
	{
		case WM_MOUSEMOVE:
		case WM_LBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_LBUTTONUP:
		case WM_RBUTTONDOWN:
		case WM_RBUTTONUP:
		case WM_RBUTTONDBLCLK:
		case WM_MBUTTONDOWN:
		case WM_MBUTTONUP:
		case WM_MBUTTONDBLCLK:
			x = LOWORD(lParam);
			y = HIWORD(lParam);

			x = x * m_dwOriginalWidth / m_dwForcedWidth;
			y = y * m_dwOriginalHeight / m_dwForcedHeight;

			lParam = MAKELPARAM(x, y);
			break;
	}

	return CallWindowProc(m_pfnOriginalWndProc, hWnd, uMsg, wParam, lParam);
}


VOID D2GI::ScaleD3D9Rect(D3D9::D3DRECT* pSrc, D3D9::D3DRECT* pOut)
{
	if (pSrc == NULL)
	{
		pOut->x1 = pOut->y1 = 0;
		pOut->x2 = m_dwForcedWidth;
		pOut->y2 = m_dwForcedHeight;
		return;
	}

	pOut->x1 = pSrc->x1 * m_dwForcedWidth / m_dwOriginalWidth;
	pOut->y1 = pSrc->y1 * m_dwForcedHeight / m_dwOriginalHeight;
	pOut->x2 = pSrc->x2 * m_dwForcedWidth / m_dwOriginalWidth;
	pOut->y2 = pSrc->y2 * m_dwForcedHeight / m_dwOriginalHeight;
}


VOID D2GI::OnTransformsSetup(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj)
{
	pmProj->_11 /= m_fAspectRatioScale;
}
