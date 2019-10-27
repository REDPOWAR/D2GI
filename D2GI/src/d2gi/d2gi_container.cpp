
#include "d2gi_container.h"
#include "d2gi_resource.h"



D2GIResourceContainer::D2GIResourceContainer(D2GI* pD2GI) : D2GIBase(pD2GI)
{
	InitializeCriticalSection(&m_sCriticalSection);
}


D2GIResourceContainer::~D2GIResourceContainer()
{
	EnterCriticalSection(&m_sCriticalSection);
	for (D2GIResource* pRes : *this)
		pRes->DetachFromContainer();
	LeaveCriticalSection(&m_sCriticalSection);

	DeleteCriticalSection(&m_sCriticalSection);
}


VOID D2GIResourceContainer::Add(D2GIResource* pRes)
{
	EnterCriticalSection(&m_sCriticalSection);
	pRes->AttachToContainer(this);
	push_back(pRes);
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GIResourceContainer::Remove(D2GIResource* pRes)
{
	INT i;

	EnterCriticalSection(&m_sCriticalSection);

	for (i = 0; i < size(); i++)
	{
		if ((*this)[i] == pRes)
		{
			pRes->DetachFromContainer();
			erase(begin() + i);
			break;
		}
	}


	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GIResourceContainer::ReleaseResources()
{
	INT i;

	EnterCriticalSection(&m_sCriticalSection);
	for (i = 0; i < size(); i++)
		(*this)[i]->ReleaseResource();
	LeaveCriticalSection(&m_sCriticalSection);
}


VOID D2GIResourceContainer::LoadResources()
{
	INT i;

	EnterCriticalSection(&m_sCriticalSection);
	for (i = 0; i < size(); i++)
		(*this)[i]->LoadResource();
	LeaveCriticalSection(&m_sCriticalSection);
}
