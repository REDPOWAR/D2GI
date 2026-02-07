#pragma once

#include "d2gi_surface.h"


class D2GIZBufferSurface : public D2GISurface
{
public:
	D2GIZBufferSurface(D2GI*, DWORD dwW, DWORD dwH, D2GIPIXELFORMAT);
	virtual ~D2GIZBufferSurface();

	virtual SURFACETYPE GetType() const override { return ST_ZBUFFER; }
	virtual void ReleaseResource(bool bResettingDevice) override {};
	virtual void LoadResource(bool bResettingDevice) override {}
};
