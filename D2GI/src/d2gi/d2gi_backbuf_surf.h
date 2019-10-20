#pragma once

#include "d2gi_surface.h"


class D2GIBackBufferSurface : public D2GISurface
{
public:
	D2GIBackBufferSurface(D2GI*);
	virtual ~D2GIBackBufferSurface();

	virtual SURFACETYPE GetType() { return ST_BACKBUFFER; }
	virtual VOID ReleaseResource() {};
	virtual VOID LoadResource() {};
};
