#pragma once

#include "../d3d/d3d_device.h"

#include "d2gi_common.h"


class D2GIDevice : public DeviceProxy, public D2GIBase
{
public:
	D2GIDevice(D2GI*);
	virtual ~D2GIDevice();

	STDMETHOD(EnumTextureFormats)(D3D7::LPD3DENUMPIXELFORMATSCALLBACK, LPVOID);
	STDMETHOD(SetTexture)(DWORD, D3D7::LPDIRECTDRAWSURFACE7);
};
