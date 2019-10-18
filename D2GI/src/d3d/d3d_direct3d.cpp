
#include "../common.h"

#include "d3d_direct3d.h"


D3DProxy::D3DProxy(IDirect3D7* pOriginal) : m_pOriginal(pOriginal)
{

}


D3DProxy::~D3DProxy()
{

}


HRESULT D3DProxy::QueryInterface(REFIID riid, LPVOID* ppvObj)
{
	Debug(TEXT("Querying interface"));
	return m_pOriginal->QueryInterface(riid, ppvObj);
}


ULONG D3DProxy::AddRef()
{
	Debug(TEXT("Adding ref"));
	return m_pOriginal->AddRef();
}


ULONG D3DProxy::Release()
{
	ULONG uRes = m_pOriginal->Release();

	if (uRes == 0)
		delete this;

	return uRes;
}


HRESULT D3DProxy::EnumDevices(LPD3DENUMDEVICESCALLBACK7 lpCallback, LPVOID lpArg)
{
	return m_pOriginal->EnumDevices(lpCallback, lpArg);
}


HRESULT D3DProxy::CreateDevice(REFCLSID iid, LPDIRECTDRAWSURFACE7 lpSurf, LPDIRECT3DDEVICE7* lpDev)
{
	return m_pOriginal->CreateDevice(iid, lpSurf, lpDev);
}


HRESULT D3DProxy::CreateVertexBuffer(LPD3DVERTEXBUFFERDESC lpDesc, LPDIRECT3DVERTEXBUFFER7* lpVB, DWORD dwFlags)
{
	Debug(TEXT("Creating vertex buffer with flags 0x%08x"), dwFlags);
	return m_pOriginal->CreateVertexBuffer(lpDesc, lpVB, dwFlags);
}


HRESULT D3DProxy::EnumZBufferFormats(REFCLSID iid, LPD3DENUMPIXELFORMATSCALLBACK lpCallback, LPVOID lpArg)
{
	Debug(TEXT("Enumerating z-buffer formats"));
	return m_pOriginal->EnumZBufferFormats(iid, lpCallback, lpArg);
}


HRESULT D3DProxy::EvictManagedTextures()
{
	Debug(TEXT("Evicting managed textures"));
	return m_pOriginal->EvictManagedTextures();
}
