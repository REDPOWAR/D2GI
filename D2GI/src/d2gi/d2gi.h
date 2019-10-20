#pragma once

#include <windows.h>

#include "../d3d9.h"

#include "d2gi_ddraw.h"


enum RENDERSTATE
{
	RS_UNKNOWN,
	RS_BACKBUFFER_STREAMING,
};


class D2GI
{
	D2GIDirectDraw* m_pDirectDrawProxy;

	HMODULE m_hD3D9Lib;
	D3D9::IDirect3D9* m_pD3D9;
	D3D9::IDirect3DDevice9* m_pDev;

	HWND m_hWnd;
	DWORD m_dwOriginalWidth, m_dwOriginalHeight, m_dwOriginalBPP;

	RENDERSTATE m_eRenderState;

	VOID LoadD3D9Library();
	VOID ResetD3D9Device();
	VOID ReleaseResources();
	VOID LoadResources();
public:
	D2GI();
	~D2GI();

	D2GIDirectDraw* GetDirectDrawProxy() { return m_pDirectDrawProxy; }
	D3D9::IDirect3D9* GetD3D9() { return m_pD3D9; }
	D3D9::IDirect3DDevice9* GetD3D9Device() { return m_pDev; }
	DWORD GetOriginalWidth() { return m_dwOriginalWidth; }
	DWORD GetOriginalHeight() { return m_dwOriginalHeight; }
	DWORD GetOriginalBPP() { return m_dwOriginalBPP; }

	VOID OnDirectDrawReleased();
	VOID OnCooperativeLevelSet(HWND, DWORD);
	VOID OnDisplayModeSet(DWORD, DWORD, DWORD, DWORD dwFlags);
	VOID OnViewportSet(D3D7::LPD3DVIEWPORT7);
	VOID OnFlip();
	VOID OnBackBufferLock();
};
