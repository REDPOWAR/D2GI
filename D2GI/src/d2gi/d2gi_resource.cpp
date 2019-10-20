
#include "d2gi_resource.h"
#include "d2gi_container.h"


D2GIResource::D2GIResource(D2GI* pD2GI)
	: D2GIBase(pD2GI), m_pContainer(NULL)
{
}


D2GIResource::~D2GIResource()
{
	if(m_pContainer != NULL)
		m_pContainer->Remove(this);
}


VOID D2GIResource::AttachToContainer(D2GIResourceContainer* pContainer)
{
	m_pContainer = pContainer;
}


VOID D2GIResource::DetachFromContainer()
{
	m_pContainer = NULL;
}
