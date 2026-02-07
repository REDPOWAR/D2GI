#pragma once

#include "d2gi_common.h"


class D2GIResourceContainer;


class D2GIResource : public D2GIBase
{
private:
	D2GIResourceContainer* m_pContainer = nullptr;

public:
	D2GIResource(D2GI*);
	~D2GIResource();

	void AttachToContainer(D2GIResourceContainer*);
	void DetachFromContainer();

	// true - device re-creation or reset; false - DirectDraw/resource release
	virtual void ReleaseResource(bool bResettingDevice) = 0;
	virtual void LoadResource(bool bResettingDevice) = 0;
};
