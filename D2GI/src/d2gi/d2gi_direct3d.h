#pragma once

#include "../d3d/d3d_direct3d.h"


class D2GIDirect3D : public D3DProxy
{
public:
	D2GIDirect3D(IDirect3D7*);
	virtual ~D2GIDirect3D();

	STDMETHOD(CreateDevice)(REFCLSID, LPDIRECTDRAWSURFACE7, LPDIRECT3DDEVICE7*);
};