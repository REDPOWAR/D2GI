
#include "../common.h"

#include "d3d_direct3d.h"


using namespace D3D7;


D3DProxy::D3DProxy() : Unknown()
{

}


D3DProxy::~D3DProxy()
{

}


HRESULT D3DProxy::EnumDevices(LPD3DENUMDEVICESCALLBACK7 lpCallback, LPVOID lpArg)
{
	return DDERR_GENERIC;
}


HRESULT D3DProxy::CreateDevice(REFCLSID iid, LPDIRECTDRAWSURFACE7 lpSurf, LPDIRECT3DDEVICE7* lpDev)
{
	return DDERR_GENERIC;
}


HRESULT D3DProxy::CreateVertexBuffer(LPD3DVERTEXBUFFERDESC lpDesc, LPDIRECT3DVERTEXBUFFER7* lpVB, DWORD dwFlags)
{
	return DDERR_GENERIC;
}


HRESULT D3DProxy::EnumZBufferFormats(REFCLSID iid, LPD3DENUMPIXELFORMATSCALLBACK lpCallback, LPVOID lpArg)
{
	return DDERR_GENERIC;
}


HRESULT D3DProxy::EvictManagedTextures()
{
	return DDERR_GENERIC;
}
