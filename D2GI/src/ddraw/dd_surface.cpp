
#include "../common.h"

#include "dd_surface.h"


using namespace D3D7;


SurfaceProxy::SurfaceProxy() : Unknown()
{

}


SurfaceProxy::~SurfaceProxy()
{

}


HRESULT SurfaceProxy::AddAttachedSurface(LPDIRECTDRAWSURFACE7 lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::AddOverlayDirtyRect(LPRECT lpRect)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::Blt(LPRECT lpDestRect, LPDIRECTDRAWSURFACE7 lpSrc, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpFX)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::BltBatch(LPDDBLTBATCH lpBatch, DWORD dwCount, DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::BltFast(DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE7 lpSrc, LPRECT lpSrcRect, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::DeleteAttachedSurface(DWORD dw, LPDIRECTDRAWSURFACE7 lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::EnumAttachedSurfaces(LPVOID lpArg, LPDDENUMSURFACESCALLBACK7 lpCallback)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::EnumOverlayZOrders(DWORD dwFlags, LPVOID lpArg, LPDDENUMSURFACESCALLBACK7 lpCallback)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::Flip(LPDIRECTDRAWSURFACE7 lpSurf, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetAttachedSurface(LPDDSCAPS2 lpCaps, LPDIRECTDRAWSURFACE7 FAR* lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetBltStatus(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetCaps(LPDDSCAPS2 lpCaps)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetClipper(LPDIRECTDRAWCLIPPER FAR* lpClipper)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetColorKey(DWORD dwFlags, LPDDCOLORKEY lpCK)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetDC(HDC FAR* lpDC)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetFlipStatus(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetOverlayPosition(LPLONG lpX, LPLONG lpY)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetPalette(LPDIRECTDRAWPALETTE FAR* lpPal)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetPixelFormat(LPDDPIXELFORMAT lpPF)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetSurfaceDesc(LPDDSURFACEDESC2 lpDesc)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::Initialize(LPDIRECTDRAW lpDD, LPDDSURFACEDESC2 lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::IsLost()
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::Lock(LPRECT lpRect, LPDDSURFACEDESC2 lpDesc, DWORD dwFlags, HANDLE h)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::ReleaseDC(HDC hDC)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::Restore()
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetClipper(LPDIRECTDRAWCLIPPER lpClipper)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetColorKey(DWORD dwFlags, LPDDCOLORKEY lpCK)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetOverlayPosition(LONG lX, LONG lY)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetPalette(LPDIRECTDRAWPALETTE lpPal)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::Unlock(LPRECT lpRect)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::UpdateOverlay(LPRECT lpSrcRect, LPDIRECTDRAWSURFACE7 lpSurf, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpFX)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::UpdateOverlayDisplay(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::UpdateOverlayZOrder(DWORD dwFlags, LPDIRECTDRAWSURFACE7 lpSurf)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetDDInterface(LPVOID FAR* lpOut)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::PageLock(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::PageUnlock(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetSurfaceDesc(LPDDSURFACEDESC2 lpDesc, DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetPrivateData(REFGUID iid, LPVOID lpData, DWORD dwSize, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetPrivateData(REFGUID iid, LPVOID lpData, LPDWORD lpSize)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::FreePrivateData(REFGUID iid)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetUniquenessValue(LPDWORD lpdw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::ChangeUniquenessValue()
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetPriority(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetPriority(LPDWORD lpdw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::SetLOD(DWORD dw)
{
	return DDERR_GENERIC;
}


HRESULT SurfaceProxy::GetLOD(LPDWORD lpdw)
{
	return DDERR_GENERIC;
}
