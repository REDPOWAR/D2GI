#pragma once

#include "../d3d/d3d_direct3d.h"


class D2GIDirect3D : public D3DProxy
{
public:
	D2GIDirect3D(D3D7::IDirect3D7*);
	virtual ~D2GIDirect3D();

	STDMETHOD(CreateDevice)(REFCLSID, D3D7::LPDIRECTDRAWSURFACE7, D3D7::LPDIRECT3DDEVICE7*);
};