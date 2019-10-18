#pragma once

#include "../d3d/d3d_device.h"


class D2GIDevice : public DeviceProxy
{
public:
	D2GIDevice(IDirect3DDevice7*);
	virtual ~D2GIDevice();

	STDMETHOD(SetTexture)(DWORD, LPDIRECTDRAWSURFACE7);
};