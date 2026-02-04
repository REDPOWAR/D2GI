#pragma once

#include "../common/d3d7.h"


class D3DProxy : public D3D7::IDirect3D7
{
public:
	static inline const GUID proxy_guid = D3D7::IID_IDirect3D7;
	using proxy_type = D3D7::IDirect3D7;

public:
	STDMETHOD(EnumDevices)(D3D7::LPD3DENUMDEVICESCALLBACK7, LPVOID)  ;
	STDMETHOD(CreateDevice)( REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*)  ;
	STDMETHOD(CreateVertexBuffer)(D3D7::LPD3DVERTEXBUFFERDESC, D3D7::LPDIRECT3DVERTEXBUFFER7*, DWORD)  ;
	STDMETHOD(EnumZBufferFormats)( REFCLSID, D3D7::LPD3DENUMPIXELFORMATSCALLBACK, LPVOID)  ;
	STDMETHOD(EvictManagedTextures)()  ;
};
