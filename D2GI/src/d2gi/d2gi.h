#pragma once

#include <vector>
#include <windows.h>

#include "d2gi_common.h"
#include "d2gi_ddraw.h"


enum RENDERSTATE
{
	RS_UNKNOWN,
	RS_BACKBUFFER_STREAMING,
	RS_PRIMARY_SURFACE_BLITTING,
	RS_BACKBUFFER_BLITTING,
	RS_3D_RENDERING,
};


class D2GISystemMemorySurface;
class D2GIPrimarySingleSurface;
class D2GIBackBufferSurface;
class D2GIBlitter;
class D2GITexture;
class D2GIStridedPrimitiveRenderer;

struct MAT3X4;
struct FRECT;

typedef std::vector<D3D9::D3DRECT> D3D9RECTVector;
typedef std::vector<BYTE>          ByteBuffer;


class D2GI
{
	D2GIDirectDraw* m_pDirectDrawProxy;

	HMODULE m_hD3D9Lib;
	D3D9::IDirect3D9* m_pD3D9;
	D3D9::IDirect3DDevice9* m_pDev;
	D3D9::IDirect3DTexture9* m_pBackBufferCopy;
	D3D9::IDirect3DSurface9* m_pBackBufferCopySurf;

	HWND m_hWnd;
	WNDPROC m_pfnOriginalWndProc;
	DWORD m_dwOriginalWidth, m_dwOriginalHeight, m_dwOriginalBPP;

	DWORD m_dwForcedWidth, m_dwForcedHeight;
	FLOAT m_fAspectRatioScale, m_fWidthScale, m_fHeightScale;

	RENDERSTATE m_eRenderState;
	BOOL m_bSceneBegun;
	BOOL m_bColorKeyEnabled;
	D2GITexture* m_lpCurrentTextures[8];

	D3D9RECTVector* m_pClearRects;
	ByteBuffer*     m_p2DBuffer;

	D2GIBlitter* m_pBlitter;
	D2GIStridedPrimitiveRenderer* m_pStridedRenderer;

	VOID LoadD3D9Library();
	VOID ResetD3D9Device();
	VOID ReleaseResources();
	VOID LoadResources();
	VOID BeginScene();
	VOID EndScene();
	VOID Present();
	VOID DrawPrimitive(D3D7::D3DPRIMITIVETYPE, DWORD dwFVF, BOOL bStrided, VOID* pVertexData,
		DWORD dwVertexCount, WORD* pIndexData, DWORD dwIndexCount, DWORD dwFlags);
	VOID ScaleFRect(FRECT* pSrc, FRECT* pOut);
	VOID ScaleRect(RECT* pSrc, RECT* pOut);
	static LRESULT CALLBACK WndProc_Static(HWND, UINT, WPARAM, LPARAM);
	LRESULT WndProc(HWND, UINT, WPARAM, LPARAM);
	VOID AttachWndProc();
	VOID DetachWndProc();
	VOID ScaleD3D9Rect(D3D9::D3DRECT* pSrc, D3D9::D3DRECT* pOut);
	VOID SetupWindow();
public:
	D2GI();
	~D2GI();

	D2GIDirectDraw* GetDirectDrawProxy() { return m_pDirectDrawProxy; }
	D3D9::IDirect3D9* GetD3D9() { return m_pD3D9; }
	D3D9::IDirect3DDevice9* GetD3D9Device() { return m_pDev; }
	DWORD GetOriginalWidth() { return m_dwOriginalWidth; }
	DWORD GetOriginalHeight() { return m_dwOriginalHeight; }
	DWORD GetOriginalBPP() { return m_dwOriginalBPP; }
	DWORD GetForcedWidth() { return m_dwForcedWidth; }
	DWORD GetForcedHeight() { return m_dwForcedHeight; }

	VOID OnDirectDrawReleased();
	VOID OnCooperativeLevelSet(HWND, DWORD);
	VOID OnDisplayModeSet(DWORD, DWORD, DWORD, DWORD dwFlags);
	VOID OnViewportSet(D3D7::LPD3DVIEWPORT7);
	VOID OnFlip();
	VOID OnBackBufferLock(BOOL bRead);
	VOID OnSysMemSurfaceBltOnPrimarySingle(D2GISystemMemorySurface*, RECT*, D2GIPrimarySingleSurface*, RECT*);
	VOID OnClear(DWORD dwCount, D3D7::LPD3DRECT lpRects, DWORD dwFlags, D3D7::D3DCOLOR col, D3D7::D3DVALUE z, DWORD dwStencil);
	VOID OnLightEnable(DWORD, BOOL);
	VOID OnSysMemSurfaceBltOnBackBuffer(D2GISystemMemorySurface*, RECT*, D2GIBackBufferSurface*, RECT*);
	VOID OnSysMemSurfaceBltOnTexture(D2GISystemMemorySurface*, RECT*, D2GITexture*, RECT*);
	VOID OnSceneBegin();
	VOID OnSceneEnd();
	VOID OnRenderStateSet(D3D7::D3DRENDERSTATETYPE, DWORD);
	VOID OnTextureStageSet(DWORD, D3D7::D3DTEXTURESTAGESTATETYPE, DWORD);
	VOID OnTextureSet(DWORD, D2GITexture*);
	BOOL OnDeviceValidate(DWORD*);
	VOID OnTransformSet(D3D7::D3DTRANSFORMSTATETYPE, D3D7::LPD3DMATRIX);
	VOID OnLightSet(DWORD, D3D7::LPD3DLIGHT7);
	VOID OnMaterialSet(D3D7::LPD3DMATERIAL7);
	VOID OnClipStatusSet(D3D7::LPD3DCLIPSTATUS);
	VOID OnIndexedPrimitiveStridedDraw(D3D7::D3DPRIMITIVETYPE, DWORD, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA, DWORD, LPWORD, DWORD, DWORD);
	VOID OnPrimitiveStridedDraw(D3D7::D3DPRIMITIVETYPE, DWORD, D3D7::LPD3DDRAWPRIMITIVESTRIDEDDATA, DWORD, DWORD);
	VOID OnPrimitiveDraw(D3D7::D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, DWORD);
	VOID OnIndexedPrimitiveDraw(D3D7::D3DPRIMITIVETYPE, DWORD, LPVOID, DWORD, LPWORD, DWORD, DWORD);
	BOOL OnRenderStateGet(D3D7::D3DRENDERSTATETYPE, DWORD*);
	VOID OnColorFillOnBackBuffer(DWORD, RECT*);
	VOID OnTransformsSetup(VOID* pThis, MAT3X4* pmView, MAT3X4* pmProj);
};
