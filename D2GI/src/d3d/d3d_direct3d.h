#pragma once

#include <windows.h>

#include "../d3d7.h"


class D3DProxy : public D3D7::IDirect3D7
{
protected:
	D3D7::IDirect3D7* m_pOriginal;
public:
	D3DProxy(D3D7::IDirect3D7*);
	virtual ~D3DProxy();

	STDMETHOD(QueryInterface)( REFIID riid, LPVOID* ppvObj)  ;
	STDMETHOD_(ULONG, AddRef)()  ;
	STDMETHOD_(ULONG, Release)()  ;

	STDMETHOD(EnumDevices)(D3D7::LPD3DENUMDEVICESCALLBACK7, LPVOID)  ;
	STDMETHOD(CreateDevice)( REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*)  ;
	STDMETHOD(CreateVertexBuffer)(D3D7::LPD3DVERTEXBUFFERDESC, D3D7::LPDIRECT3DVERTEXBUFFER7*, DWORD)  ;
	STDMETHOD(EnumZBufferFormats)( REFCLSID, D3D7::LPD3DENUMPIXELFORMATSCALLBACK, LPVOID)  ;
	STDMETHOD(EvictManagedTextures)()  ;
};
