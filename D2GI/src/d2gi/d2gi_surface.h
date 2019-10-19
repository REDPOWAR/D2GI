#pragma once

#include "../ddraw/dd_surface.h"

#include "d2gi_common.h"
#include "d2gi_resource.h"


enum SURFACETYPE
{
	ST_PRIMARY_FLIPPABLE,
	ST_BACKBUFFER,
	ST_PRIMARY_SINGLE,
	ST_ZBUFFER,
	ST_TEXTURE,
};


class D2GISurface : public SurfaceProxy, public D2GIResource
{
public:
	D2GISurface(D2GI*);
	virtual ~D2GISurface();

	virtual SURFACETYPE GetType() = 0;
};
