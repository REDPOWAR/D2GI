
#include "d2gi_container.h"
#include "d2gi_resource.h"



D2GIResourceContainer::D2GIResourceContainer(D2GI* pD2GI) : D2GIBase(pD2GI)
{

}


D2GIResourceContainer::~D2GIResourceContainer()
{
	for (D2GIResource* pRes : *this)
		pRes->DetachFromContainer();
}


VOID D2GIResourceContainer::Add(D2GIResource* pRes)
{
	pRes->AttachToContainer(this);
	push_back(pRes);
}


VOID D2GIResourceContainer::Remove(D2GIResource* pRes)
{
	INT i;

	for (i = 0; i < size(); i++)
	{
		if ((*this)[i] == pRes)
		{
			pRes->DetachFromContainer();
			erase(begin() + i);
			break;
		}
	}
}


VOID D2GIResourceContainer::ReleaseResources()
{
	INT i;

	for (i = 0; i < size(); i++)
		(*this)[i]->ReleaseResource();
}


VOID D2GIResourceContainer::LoadResources()
{
	INT i;

	for (i = 0; i < size(); i++)
		(*this)[i]->LoadResource();
}
