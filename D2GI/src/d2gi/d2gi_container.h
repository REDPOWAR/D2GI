#pragma once

#include <vector>

#include "d2gi_common.h"


class D2GIResource;


class D2GIResourceContainer
{
private:
	CRITICAL_SECTION m_sCriticalSection;
	std::vector<D2GIResource*> m_resources;

public:
	D2GIResourceContainer();
	~D2GIResourceContainer();

	void Add(D2GIResource*);
	void Remove(D2GIResource*);
	void ReleaseResources(bool bResettingDevice);
	void LoadResources(bool bResettingDevice);
};
