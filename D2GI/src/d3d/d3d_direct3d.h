#pragma once

#include "../common/common.h"
#include "../common/d3d7.h"


class D3DProxy : public D3D7::IDirect3D7, public Unknown
{
public:
	D3DProxy();
	virtual ~D3DProxy();

	STDMETHOD(QueryInterface)(REFIID riid, LPVOID* ppvObj) { return Unknown::QueryInterface(riid, ppvObj); };
	STDMETHOD_(ULONG, AddRef)() { return Unknown::AddRef(); };
	STDMETHOD_(ULONG, Release)() { return Unknown::Release(); };

	STDMETHOD(EnumDevices)(D3D7::LPD3DENUMDEVICESCALLBACK7, LPVOID)  ;
	STDMETHOD(CreateDevice)( REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*)  ;
	STDMETHOD(CreateVertexBuffer)(D3D7::LPD3DVERTEXBUFFERDESC, D3D7::LPDIRECT3DVERTEXBUFFER7*, DWORD)  ;
	STDMETHOD(EnumZBufferFormats)( REFCLSID, D3D7::LPD3DENUMPIXELFORMATSCALLBACK, LPVOID)  ;
	STDMETHOD(EvictManagedTextures)()  ;
};
