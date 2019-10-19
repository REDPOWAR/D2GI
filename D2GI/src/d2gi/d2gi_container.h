#pragma once

#include <vector>

#include "d2gi_common.h"


class D2GIResource;


class D2GIResourceContainer : public std::vector<D2GIResource*>, public D2GIBase
{
public:
	D2GIResourceContainer(D2GI*);
	~D2GIResourceContainer();

	VOID Add(D2GIResource*);
	VOID Remove(D2GIResource*);
	VOID ReleaseResources();
	VOID LoadResources();
};
