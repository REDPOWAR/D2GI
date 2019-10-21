#pragma once

#include "d2gi_surface.h"


class D2GIZBufferSurface : public D2GISurface
{
public:
	D2GIZBufferSurface(D2GI*);
	virtual ~D2GIZBufferSurface();

	virtual SURFACETYPE GetType() { return ST_ZBUFFER; }
	virtual VOID ReleaseResource() {};
	virtual VOID LoadResource() {}

	STDMETHOD(IsLost)();
};
