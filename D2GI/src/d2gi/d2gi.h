#pragma once

#include "d2gi_ddraw.h"


struct IDirect3D9;
struct IDirect3DDevice9;


class D2GI
{
	D2GIDirectDraw* m_pDirectDrawProxy;

	HMODULE m_hD3D9Lib;
	IDirect3D9* m_pD3D9;
	IDirect3DDevice9* m_pDev;

	HWND m_hWnd;
	DWORD m_dwOriginalWidth, m_dwOriginalHeight, m_dwOriginalBPP;

	VOID LoadD3D9Library();
	VOID ResetD3D9Device();
public:
	D2GI();
	~D2GI();

	D2GIDirectDraw* GetDirectDrawProxy() { return m_pDirectDrawProxy; }

	VOID OnDirectDrawReleased();
	VOID OnCooperativeLevelSet(HWND, DWORD);
	VOID OnDisplayModeSet(DWORD, DWORD, DWORD, DWORD dwFlags);
};