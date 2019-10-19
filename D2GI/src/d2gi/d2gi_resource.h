#pragma once

#include "d2gi_common.h"


class D2GIResourceContainer;


class D2GIResource : public D2GIBase
{
protected:
	D2GIResourceContainer* m_pContainer;
public:
	D2GIResource(D2GI*);
	~D2GIResource();

	VOID AttachToContainer(D2GIResourceContainer*);
	VOID DetachFromContainer();
	virtual VOID ReleaseResource() = 0;
	virtual VOID LoadResource() = 0;
};
