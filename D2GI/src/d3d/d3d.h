#pragma once

#include <d3d.h>


class D3DProxy : public IDirect3D7
{
protected:
	IDirect3D7* m_pOriginal;
public:
	D3DProxy(IDirect3D7*);
	virtual ~D3DProxy();

	STDMETHOD(QueryInterface)( REFIID riid, LPVOID* ppvObj)  ;
	STDMETHOD_(ULONG, AddRef)()  ;
	STDMETHOD_(ULONG, Release)()  ;

	STDMETHOD(EnumDevices)( LPD3DENUMDEVICESCALLBACK7, LPVOID)  ;
	STDMETHOD(CreateDevice)( REFCLSID, LPDIRECTDRAWSURFACE7, LPDIRECT3DDEVICE7*)  ;
	STDMETHOD(CreateVertexBuffer)( LPD3DVERTEXBUFFERDESC, LPDIRECT3DVERTEXBUFFER7*, DWORD)  ;
	STDMETHOD(EnumZBufferFormats)( REFCLSID, LPD3DENUMPIXELFORMATSCALLBACK, LPVOID)  ;
	STDMETHOD(EvictManagedTextures)()  ;
};
