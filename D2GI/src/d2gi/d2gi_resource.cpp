
#include "d2gi_resource.h"
#include "d2gi_container.h"


D2GIResource::D2GIResource(D2GI* pD2GI)
	: D2GIBase(pD2GI)
{
}


D2GIResource::~D2GIResource()
{
	if(m_pContainer != nullptr)
		m_pContainer->Remove(this);
}


void D2GIResource::AttachToContainer(D2GIResourceContainer* pContainer)
{
	m_pContainer = pContainer;
}


void D2GIResource::DetachFromContainer()
{
	m_pContainer = nullptr;
}
