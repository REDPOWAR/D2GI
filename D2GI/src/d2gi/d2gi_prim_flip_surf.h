#pragma once

#include "d2gi_surface.h"


class D2GIPrimaryFlippableSurface : public D2GISurface
{
public:
	D2GIPrimaryFlippableSurface(D2GI*);
	virtual ~D2GIPrimaryFlippableSurface();

	virtual SURFACETYPE GetType() { return ST_PRIMARY_FLIPPABLE; }
	virtual VOID ReleaseResource() {};
	virtual VOID LoadResource() {};
};
