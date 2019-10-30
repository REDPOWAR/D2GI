
#include "../common.h"
#include "../utils.h"

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
	: m_hD3D9Lib(NULL), m_pD3D9(NULL), m_pDev(NULL), m_eRenderState(RS_UNKNOWN), 
	m_bSceneBegun(FALSE), m_bColorKeyEnabled(FALSE)
{
	ZeroMemory(m_lpCurrentTextures, sizeof(m_lpCurrentTextures));

	InitializeCriticalSection(&m_sCriticalSection);

	m_pDirectDrawProxy = new D2GIDirectDraw(this);
	m_pBlitter = new D2GIBlitter(this);
	m_pStridedRenderer = new D2GIStridedPrimitiveRenderer(this);

	LoadD3D9Library();
}


D2GI::~D2GI()
{
	DEL(m_pStridedRenderer);
	DEL(m_pBlitter);
	RELEASE(m_pDev);
	RELEASE(m_pD3D9);
	FreeLibrary(m_hD3D9Lib);
	DeleteCriticalSection(&m_sCriticalSection);
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

	GetSystemDirectory(szPath, MAX_PATH);
	_tcscat(szPath, TEXT("\\d3d9.dll"));

	m_hD3D9Lib = LoadLibrary(szPath);

	pfnDirect3DCreate9 = (DIRECT3DCREATE9)GetProcAddress(m_hD3D9Lib, "Direct3DCreate9");

	m_pD3D9 = pfnDirect3DCreate9(D3D_SDK_VERSION);
}


VOID D2GI::OnCooperativeLevelSet(HWND hWnd, DWORD dwFlags)
{
	m_hWnd = hWnd;
}


VOID D2GI::OnDisplayModeSet(DWORD dwWidth, DWORD dwHeight, DWORD dwBPP, DWORD dwFlags)
{
	m_dwOriginalWidth = dwWidth;
	m_dwOriginalHeight = dwHeight;
	m_dwOriginalBPP = dwBPP;

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
	RELEASE(m_pDev);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowLong(m_hWnd, GWL_EXSTYLE, 0);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_dwOriginalWidth, m_dwOriginalHeight, 0);

	ZeroMemory(&sParams, sizeof(sParams));
	sParams.AutoDepthStencilFormat = D3D9::D3DFMT_D24X8;
	sParams.EnableAutoDepthStencil = TRUE;
	sParams.BackBufferCount = 1;
	sParams.BackBufferWidth = m_dwOriginalWidth;
	sParams.BackBufferHeight = m_dwOriginalHeight;
	sParams.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	sParams.SwapEffect = D3D9::D3DSWAPEFFECT_DISCARD;
	sParams.Windowed = TRUE;

	m_pD3D9->CreateDevice(0, D3D9::D3DDEVTYPE_HAL, m_hWnd, 
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_MULTITHREADED, &sParams, &m_pDev);
	LoadResources();
}


VOID D2GI::OnViewportSet(D3D7::LPD3DVIEWPORT7 pVP)
{
	D3D9::D3DVIEWPORT9 sD3D9Viewport;

	EnterCriticalSection(&m_sCriticalSection);

	sD3D9Viewport.X = pVP->dwX;
	sD3D9Viewport.Y = pVP->dwY;
	sD3D9Viewport.Width = pVP->dwWidth;
	sD3D9Viewport.Height = pVP->dwHeight;
	sD3D9Viewport.MinZ = pVP->dvMinZ;
	sD3D9Viewport.MaxZ = pVP->dvMaxZ;

	m_pDev->SetViewport(&sD3D9Viewport);

	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnBackBufferLock()
{
	m_eRenderState = RS_BACKBUFFER_STREAMING;
}


VOID D2GI::OnFlip()
{
	EnterCriticalSection(&m_sCriticalSection);

	if (m_eRenderState == RS_BACKBUFFER_STREAMING)
	{
		D2GIPrimaryFlippableSurface* pPrimSurf = m_pDirectDrawProxy->GetPrimaryFlippableSurface();
		D3D9::IDirect3DSurface9* pSurf = pPrimSurf->GetBackBufferSurface()->GetD3D9Surface();
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
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnSysMemSurfaceBltOnPrimarySingle(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GIPrimarySingleSurface* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	EnterCriticalSection(&m_sCriticalSection);

	m_eRenderState = RS_PRIMARY_SURFACE_BLITTING;

	if (m_dwOriginalBPP == 8)
	{
		pSrc->UpdateWithPalette(pDst->GetPalette());
		m_pDev->GetRenderTarget(0, &pRT);
		m_pDev->StretchRect(pSrc->GetD3D9Surface(), pSrcRT, pRT, pDstRT, D3D9::D3DTEXF_POINT);
		Present();

		pRT->Release();
	}


	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnClear(DWORD dwCount, D3D7::LPD3DRECT lpRects, DWORD dwFlags, D3D7::D3DCOLOR col, D3D7::D3DVALUE z, DWORD dwStencil)
{
	EnterCriticalSection(&m_sCriticalSection);
	m_pDev->Clear(dwCount, (D3D9::D3DRECT*)lpRects, dwFlags, col, z, dwStencil);
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnLightEnable(DWORD i, BOOL bEnable)
{
	EnterCriticalSection(&m_sCriticalSection);
	m_pDev->LightEnable(i, bEnable);
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnSysMemSurfaceBltOnBackBuffer(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GIBackBufferSurface* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	EnterCriticalSection(&m_sCriticalSection);

	m_eRenderState = RS_BACKBUFFER_BLITTING;

	//BeginScene();
	m_pDev->GetRenderTarget(0, &pRT);
	if(!m_bSceneBegun)
		m_pDev->BeginScene();

	if (pSrc->HasColorKey())
		m_pBlitter->BlitWithColorKey(pRT, pDstRT, pSrc->GetD3D9Texture(), pSrcRT, pSrc->GetColorKeyValue());
	else
		m_pBlitter->Blit(pRT, pDstRT, pSrc->GetD3D9Texture(), pSrcRT);

	if (!m_bSceneBegun)
		m_pDev->EndScene();

	pRT->Release();
	//EndScene();


	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnSysMemSurfaceBltOnTexture(D2GISystemMemorySurface* pSrc, RECT* pSrcRT, D2GITexture* pDst, RECT* pDstRT)
{
	D3D9::IDirect3DSurface9* pRT;

	// MULTITHREADED_ACCESS


	/*EnterCriticalSection(&m_sCriticalSection);


	pDst->MakeRenderTarget();

	if(!m_bSceneBegun)
		m_pDev->BeginScene();
	m_pDev->GetRenderTarget(0, &pRT);
	m_pBlitter->Blit(pDst->GetD3D9Surface(), pDstRT, pSrc->GetD3D9Texture(), pSrcRT);
	m_pDev->SetRenderTarget(0, pRT);

	if (!m_bSceneBegun)
		m_pDev->EndScene();
	pRT->Release();


	LeaveCriticalSection(&m_sCriticalSection);*/

//	LeaveCriticalSection(&m_sCriticalSection);
	//m_pDev->StretchRect(pSrc->GetD3D9Surface(), pSrcRT, pDst->GetD3D9Surface(), pDstRT, D3D9::D3DTEXF_POINT);

	if (pDst->GetWidth() != pSrc->GetWidth() || pDst->GetHeight() != pSrc->GetHeight())
		return;

	VOID* pData;
	D3D9::D3DLOCKED_RECT rt;

	pDst->GetD3D9Texture()->LockRect(0, &rt, NULL, D3DLOCK_DISCARD);
	INT i, j;

	for (i = 0; i < (INT)pDst->GetHeight(); i++)
		CopyMemory((BYTE*)rt.pBits + i * rt.Pitch, 
		(BYTE*)pSrc->GetData() + i * pSrc->GetDataPitch(), pSrc->GetDataPitch());
	pDst->GetD3D9Texture()->UnlockRect(0);
}


VOID D2GI::OnSceneBegin()
{
	EnterCriticalSection(&m_sCriticalSection);

	m_eRenderState = RS_3D_RENDERING;
	BeginScene();
	LeaveCriticalSection(&m_sCriticalSection);
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

	EnterCriticalSection(&m_sCriticalSection);

	m_eRenderState = RS_3D_RENDERING;
	EndScene();
	LeaveCriticalSection(&m_sCriticalSection);
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

	EnterCriticalSection(&m_sCriticalSection);

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


	LeaveCriticalSection(&m_sCriticalSection);
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

	EnterCriticalSection(&m_sCriticalSection);


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


	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnTextureSet(DWORD i, D2GITexture* pTex)
{
	EnterCriticalSection(&m_sCriticalSection);

	m_lpCurrentTextures[i] = pTex;

	m_pDev->SetTexture(i, pTex == NULL ? NULL : pTex->GetD3D9Texture());
	LeaveCriticalSection(&m_sCriticalSection);
}


BOOL D2GI::OnDeviceValidate(DWORD* pdw)
{

	EnterCriticalSection(&m_sCriticalSection);

	BOOL bRes = SUCCEEDED(m_pDev->ValidateDevice(pdw));


	LeaveCriticalSection(&m_sCriticalSection);

	return bRes;
}


VOID D2GI::OnTransformSet(D3D7::D3DTRANSFORMSTATETYPE eType, D3D7::D3DMATRIX* pMatrix)
{

	EnterCriticalSection(&m_sCriticalSection);

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
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnLightSet(DWORD i, D3D7::LPD3DLIGHT7 pLight)
{

	EnterCriticalSection(&m_sCriticalSection);

	m_pDev->SetLight(i, (D3D9::D3DLIGHT9*)pLight);
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnMaterialSet(D3D7::LPD3DMATERIAL7 pMaterial)
{

	EnterCriticalSection(&m_sCriticalSection);

	m_pDev->SetMaterial((D3D9::D3DMATERIAL9*)pMaterial);
	LeaveCriticalSection(&m_sCriticalSection);
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
	DWORD dwATEnable, dwATFunc, dwATRef;

	EnterCriticalSection(&m_sCriticalSection);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwATEnable);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAFUNC, &dwATFunc);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAREF, &dwATRef);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, TRUE);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, D3D9::D3DCMP_GREATEREQUAL);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, 0x00000080);
	}
	m_pStridedRenderer->DrawIndexedPrimitiveStrided(pt, dwFVF, pData, dwCount, pIdx, dwIdxCount, dwFlags);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwATEnable);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, dwATFunc);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, dwATRef);
	}


	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnPrimitiveStridedDraw(
	D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA pData,
	DWORD dwCount, DWORD dwFlags)
{
	DWORD dwATEnable, dwATFunc, dwATRef;

	EnterCriticalSection(&m_sCriticalSection);


	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwATEnable);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAFUNC, &dwATFunc);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAREF, &dwATRef);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, TRUE);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, D3D9::D3DCMP_GREATEREQUAL);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, 0x00000080);
	}
	m_pStridedRenderer->DrawPrimitiveStrided(pt, dwFVF, pData, dwCount, dwFlags);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwATEnable);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, dwATFunc);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, dwATRef);
	}


	LeaveCriticalSection(&m_sCriticalSection);
}



VOID D2GI::OnPrimitiveDraw(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, DWORD dwFlags)
{
	UINT uVertexStride, uPrimCount;
	DWORD dwATEnable, dwATFunc, dwATRef;

	EnterCriticalSection(&m_sCriticalSection);


	uVertexStride = CalcFVFStride(dwFVF);
	uPrimCount = CalcPrimitiveCount(pt, dwVertCount);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwATEnable);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAFUNC, &dwATFunc);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAREF, &dwATRef);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, TRUE);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, D3D9::D3DCMP_GREATEREQUAL);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, 0x00000080);
	}
	m_pDev->SetFVF(dwFVF);
	m_pDev->DrawPrimitiveUP((D3D9::D3DPRIMITIVETYPE)pt, uPrimCount, pVerts, uVertexStride);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwATEnable);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, dwATFunc);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, dwATRef);
	}


	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GI::OnIndexedPrimitiveDraw(D3D7::D3DPRIMITIVETYPE pt, DWORD dwFVF, LPVOID pVerts, DWORD dwVertCount, LPWORD pIdx, DWORD dwIdxCount, DWORD dwFlags)
{
	UINT uVertexStride, uPrimCount;
	DWORD dwATEnable, dwATFunc, dwATRef;

	EnterCriticalSection(&m_sCriticalSection);


	uVertexStride = CalcFVFStride(dwFVF);
	uPrimCount = CalcIndexedPrimitiveCount(pt, dwIdxCount);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHATESTENABLE, &dwATEnable);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAFUNC, &dwATFunc);
		m_pDev->GetRenderState(D3D9::D3DRS_ALPHAREF, &dwATRef);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, TRUE);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, D3D9::D3DCMP_GREATEREQUAL);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, 0x00000080);
	}
	m_pDev->SetFVF(dwFVF);
	m_pDev->DrawIndexedPrimitiveUP((D3D9::D3DPRIMITIVETYPE)pt, 0, dwVertCount, 
		uPrimCount, pIdx, D3D9::D3DFMT_INDEX16, pVerts, uVertexStride);

	if (m_bColorKeyEnabled && m_lpCurrentTextures[0] && m_lpCurrentTextures[0]->HasColorKey())
	{
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHATESTENABLE, dwATEnable);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAFUNC, dwATFunc);
		m_pDev->SetRenderState(D3D9::D3DRS_ALPHAREF, dwATRef);
	}


	LeaveCriticalSection(&m_sCriticalSection);
}


BOOL D2GI::OnRenderStateGet(D3D7::D3DRENDERSTATETYPE eState, DWORD* pValue)
{

	EnterCriticalSection(&m_sCriticalSection);

	switch (eState)
	{
	case D3D7::D3DRENDERSTATE_CULLMODE:
		m_pDev->GetRenderState(D3D9::D3DRS_CULLMODE, pValue);

		LeaveCriticalSection(&m_sCriticalSection);
		return TRUE;
	}

	LeaveCriticalSection(&m_sCriticalSection);
	return FALSE;
}
