
#include "d2gi_container.h"
#include "d2gi_resource.h"

#include <algorithm>


D2GIResourceContainer::D2GIResourceContainer()
{
	InitializeCriticalSection(&m_sCriticalSection);
}


D2GIResourceContainer::~D2GIResourceContainer()
{
	EnterCriticalSection(&m_sCriticalSection);
	for (D2GIResource* pRes : m_resources)
		pRes->DetachFromContainer();
	LeaveCriticalSection(&m_sCriticalSection);

	DeleteCriticalSection(&m_sCriticalSection);
}


void D2GIResourceContainer::Add(D2GIResource* pRes)
{
	pRes->AttachToContainer(this);
	EnterCriticalSection(&m_sCriticalSection);
	m_resources.push_back(pRes);
	LeaveCriticalSection(&m_sCriticalSection);
}


void D2GIResourceContainer::Remove(D2GIResource* pRes)
{
	EnterCriticalSection(&m_sCriticalSection);

	auto it = std::find(m_resources.begin(), m_resources.end(), pRes);
	if (it != m_resources.end())
	{
		m_resources.erase(it);
	}

	LeaveCriticalSection(&m_sCriticalSection);

	pRes->DetachFromContainer();
}


void D2GIResourceContainer::ReleaseResources()
{
	EnterCriticalSection(&m_sCriticalSection);
	for (D2GIResource* pRes : m_resources)
		pRes->ReleaseResource();
	LeaveCriticalSection(&m_sCriticalSection);
}


void D2GIResourceContainer::LoadResources()
{
	EnterCriticalSection(&m_sCriticalSection);
	for (D2GIResource* pRes : m_resources)
		pRes->LoadResource();
	LeaveCriticalSection(&m_sCriticalSection);
}
