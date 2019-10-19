#pragma once

#include "../d3d/d3d_direct3d.h"

#include "d2gi_common.h"


class D2GIDirect3D : public D3DProxy, public D2GIBase
{
public:
	D2GIDirect3D(D2GI*);
	virtual ~D2GIDirect3D();

	STDMETHOD(CreateDevice)(REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*);
	STDMETHOD(EnumDevices)(D3D7::LPD3DENUMDEVICESCALLBACK7, LPVOID);
};