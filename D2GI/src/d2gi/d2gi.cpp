
#include "../common.h"

#include "d2gi.h"
#include "d2gi_ddraw.h"
#include "d2gi_prim_flip_surf.h"
#include "d2gi_backbuf_surf.h"


D2GI::D2GI()
	: m_hD3D9Lib(NULL), m_pD3D9(NULL), m_pDev(NULL), m_eRenderState(RS_UNKNOWN)
{
	m_pDirectDrawProxy = new D2GIDirectDraw(this);

	LoadD3D9Library();
}


D2GI::~D2GI()
{
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
}


VOID D2GI::LoadResources()
{
	m_pDirectDrawProxy->LoadResources();
}


VOID D2GI::ResetD3D9Device()
{
	D3D9::D3DPRESENT_PARAMETERS sParams;

	ReleaseResources();
	RELEASE(m_pDev);

	SetWindowLong(m_hWnd, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	SetWindowPos(m_hWnd, HWND_TOP, 0, 0, m_dwOriginalWidth, m_dwOriginalHeight, 0);

	ZeroMemory(&sParams, sizeof(sParams));
	sParams.AutoDepthStencilFormat = D3D9::D3DFMT_D24X8;
	sParams.EnableAutoDepthStencil = TRUE;
	sParams.BackBufferCount = 1;
	sParams.BackBufferWidth = m_dwOriginalWidth;
	sParams.BackBufferHeight = m_dwOriginalHeight;
	sParams.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	sParams.SwapEffect = D3D9::D3DSWAPEFFECT_DISCARD;
	sParams.Windowed = TRUE;

	m_pD3D9->CreateDevice(0, D3D9::D3DDEVTYPE_HAL, m_hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &sParams, &m_pDev);
	LoadResources();
}


VOID D2GI::OnViewportSet(D3D7::LPD3DVIEWPORT7 pVP)
{
	D3D9::D3DVIEWPORT9 sD3D9Viewport;

	sD3D9Viewport.X = pVP->dwX;
	sD3D9Viewport.Y = pVP->dwY;
	sD3D9Viewport.Width = pVP->dwWidth;
	sD3D9Viewport.Height = pVP->dwHeight;
	sD3D9Viewport.MinZ = pVP->dvMinZ;
	sD3D9Viewport.MaxZ = pVP->dvMaxZ;

	m_pDev->SetViewport(&sD3D9Viewport);
}


VOID D2GI::OnBackBufferLock()
{
	m_eRenderState = RS_BACKBUFFER_STREAMING;
}


VOID D2GI::OnFlip()
{
	if (m_eRenderState == RS_BACKBUFFER_STREAMING)
	{
		D2GIPrimaryFlippableSurface* pPrimSurf = m_pDirectDrawProxy->GetPrimaryFlippableSurface();
		D3D9::IDirect3DSurface9* pSurf = pPrimSurf->GetBackBufferSurface()->GetD3D9Surface();
		D3D9::IDirect3DSurface9* pRT;

		m_pDev->GetRenderTarget(0, &pRT);
		HRESULT hRes = m_pDev->StretchRect(pSurf, NULL, pRT, NULL, D3D9::D3DTEXF_POINT);
		m_pDev->Present(NULL, NULL, NULL, NULL);

		pRT->Release();
	}
}
