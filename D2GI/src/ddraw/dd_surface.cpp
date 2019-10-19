
#include "../common.h"

#include "dd_surface.h"


using namespace D3D7;


SurfaceProxy::SurfaceProxy(IDirectDrawSurface7* pOriginal) : m_pOriginal(pOriginal)
{

}


SurfaceProxy::~SurfaceProxy()
{

}


HRESULT SurfaceProxy::QueryInterface(REFIID riid, LPVOID FAR* ppvObj)
{
	return m_pOriginal->QueryInterface(riid, ppvObj);
}


ULONG SurfaceProxy::AddRef()
{
	return m_pOriginal->AddRef();
}


ULONG SurfaceProxy::Release()
{
	ULONG uRes = m_pOriginal->Release();

	if (uRes == 0)
		delete this;

	return uRes;
}


HRESULT SurfaceProxy::AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpSurf)
{
	return m_pOriginal->AddAttachedSurface(lpSurf);
}


HRESULT SurfaceProxy::AddOverlayDirtyRect(LPRECT lpRect)
{
	return m_pOriginal->AddOverlayDirtyRect(lpRect);
}


HRESULT SurfaceProxy::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpSrc, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpFX)
{
	return m_pOriginal->Blt(lpDestRect, lpSrc, lpSrcRect, dwFlags, lpFX);
}


HRESULT SurfaceProxy::BltBatch(LPDDBLTBATCH lpBatch, DWORD dwCount, DWORD dw)
{
	return m_pOriginal->BltBatch(lpBatch, dwCount, dw);
}


HRESULT SurfaceProxy::BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpSrc, LPRECT lpSrcRect, DWORD dwFlags)
{
	return m_pOriginal->BltFast(dwX, dwY, lpSrc, lpSrcRect, dwFlags);
}


HRESULT SurfaceProxy::DeleteAttachedSurface(DWORD dw, LPDIRECTDRAWSURFACE7 lpSurf)
{
	return m_pOriginal->DeleteAttachedSurface(dw, lpSurf);
}


HRESULT SurfaceProxy::EnumAttachedSurfaces(LPVOID lpArg, LPDDENUMSURFACESCALLBACK7 lpCallback)
{
	return m_pOriginal->EnumAttachedSurfaces(lpArg, lpCallback);
}


HRESULT SurfaceProxy::EnumOverlayZOrders(DWORD dwFlags, LPVOID lpArg, LPDDENUMSURFACESCALLBACK7 lpCallback)
{
	return m_pOriginal->EnumOverlayZOrders(dwFlags, lpArg, lpCallback);
}


HRESULT SurfaceProxy::Flip(LPDIRECTDRAWSURFACE7 lpSurf, DWORD dwFlags)
{
	return m_pOriginal->Flip(lpSurf, dwFlags);
}


HRESULT SurfaceProxy::GetAttachedSurface(LPDDSCAPS2 lpCaps, LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return m_pOriginal->GetAttachedSurface(lpCaps, lpSurf);
}


HRESULT SurfaceProxy::GetBltStatus(DWORD dw)
{
	return m_pOriginal->GetBltStatus(dw);
}


HRESULT SurfaceProxy::GetCaps(LPDDSCAPS2 lpCaps)
{
	return m_pOriginal->GetCaps(lpCaps);
}


HRESULT SurfaceProxy::GetClipper(LPDIRECTDRAWCLIPPER FAR* lpClipper)
{
	return m_pOriginal->GetClipper(lpClipper);
}


HRESULT SurfaceProxy::GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpCK)
{
	return m_pOriginal->GetColorKey(dwFlags, lpCK);
}


HRESULT SurfaceProxy::GetDC(HDC FAR* lpDC)
{
	return m_pOriginal->GetDC(lpDC);
}


HRESULT SurfaceProxy::GetFlipStatus(DWORD dw)
{
	return m_pOriginal->GetFlipStatus(dw);
}


HRESULT SurfaceProxy::GetOverlayPosition(LPLONG lpX, LPLONG lpY)
{
	return m_pOriginal->GetOverlayPosition(lpX, lpY);
}


HRESULT SurfaceProxy::GetPalette(LPDIRECTDRAWPALETTE FAR* lpPal)
{
	return m_pOriginal->GetPalette(lpPal);
}


HRESULT SurfaceProxy::GetPixelFormat(LPDDPIXELFORMAT lpPF)
{
	return m_pOriginal->GetPixelFormat(lpPF);
}


HRESULT SurfaceProxy::GetSurfaceDesc(LPDDSURFACEDESC2 lpDesc)
{
	return m_pOriginal->GetSurfaceDesc(lpDesc);
}


HRESULT SurfaceProxy::Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpSurf)
{
	return m_pOriginal->Initialize(lpDD, lpSurf);
}


HRESULT SurfaceProxy::IsLost()
{
	return m_pOriginal->IsLost();
}


HRESULT SurfaceProxy::Lock(LPRECT lpRect, LPDDSURFACEDESC2 lpDesc, DWORD dwFlags, HANDLE h)
{
	Debug(TEXT("Locking 0x%08x surface"), GetOriginal());
	return m_pOriginal->Lock(lpRect, lpDesc, dwFlags, h);
}


HRESULT SurfaceProxy::ReleaseDC(HDC hDC)
{
	return m_pOriginal->ReleaseDC(hDC);
}


HRESULT SurfaceProxy::Restore()
{
	return m_pOriginal->Restore();
}


HRESULT SurfaceProxy::SetClipper(LPDIRECTDRAWCLIPPER lpClipper)
{
	return m_pOriginal->SetClipper(lpClipper);
}


HRESULT SurfaceProxy::SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpCK)
{
	return m_pOriginal->SetColorKey(dwFlags, lpCK);
}


HRESULT SurfaceProxy::SetOverlayPosition(LONG lX, LONG lY)
{
	return m_pOriginal->SetOverlayPosition(lX, lY);
}


HRESULT SurfaceProxy::SetPalette(LPDIRECTDRAWPALETTE lpPal)
{
	Debug(TEXT("Setting palette for 0x%08x"), GetOriginal());
	return m_pOriginal->SetPalette(lpPal);
}


HRESULT SurfaceProxy::Unlock(LPRECT lpRect)
{
	return m_pOriginal->Unlock(lpRect);
}


HRESULT SurfaceProxy::UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpSurf, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpFX)
{
	return m_pOriginal->UpdateOverlay(lpSrcRect, lpSurf, lpDestRect, dwFlags, lpFX);
}


HRESULT SurfaceProxy::UpdateOverlayDisplay(DWORD dw)
{
	return m_pOriginal->UpdateOverlayDisplay(dw);
}


HRESULT SurfaceProxy::UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpSurf)
{
	return m_pOriginal->UpdateOverlayZOrder(dwFlags, lpSurf);
}


HRESULT SurfaceProxy::GetDDInterface(LPVOID FAR* lpOut)
{
	return m_pOriginal->GetDDInterface(lpOut);
}


HRESULT SurfaceProxy::PageLock(DWORD dw)
{
	return m_pOriginal->PageLock(dw);
}


HRESULT SurfaceProxy::PageUnlock(DWORD dw)
{
	return m_pOriginal->PageUnlock(dw);
}


HRESULT SurfaceProxy::SetSurfaceDesc(LPDDSURFACEDESC2 lpDesc, DWORD dw)
{
	return m_pOriginal->SetSurfaceDesc(lpDesc, dw);
}


HRESULT SurfaceProxy::SetPrivateData(REFGUID iid, LPVOID lpData, DWORD dwSize, DWORD dwFlags)
{
	return m_pOriginal->SetPrivateData(iid, lpData, dwSize, dwFlags);
}


HRESULT SurfaceProxy::GetPrivateData(REFGUID iid, LPVOID lpData, LPDWORD lpSize)
{
	return m_pOriginal->GetPrivateData(iid, lpData, lpSize);
}


HRESULT SurfaceProxy::FreePrivateData(REFGUID iid)
{
	return m_pOriginal->FreePrivateData(iid);
}


HRESULT SurfaceProxy::GetUniquenessValue(LPDWORD lpdw)
{
	return m_pOriginal->GetUniquenessValue(lpdw);
}


HRESULT SurfaceProxy::ChangeUniquenessValue()
{
	return m_pOriginal->ChangeUniquenessValue();
}


HRESULT SurfaceProxy::SetPriority(DWORD dw)
{
	return m_pOriginal->SetPriority(dw);
}


HRESULT SurfaceProxy::GetPriority(LPDWORD lpdw)
{
	return m_pOriginal->GetPriority(lpdw);
}


HRESULT SurfaceProxy::SetLOD(DWORD dw)
{
	return m_pOriginal->SetLOD(dw);
}


HRESULT SurfaceProxy::GetLOD(LPDWORD lpdw)
{
	return m_pOriginal->GetLOD(lpdw);
}
