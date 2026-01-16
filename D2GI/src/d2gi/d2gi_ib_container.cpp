
#include "../common/logger.h"
#include <algorithm>
#include "d2gi_ib_container.h"


#define DEFAULT_IB_SIZE  (2u * 256u * 256u)


D2GIIndexBufferContainer::D2GIIndexBufferContainer(D2GI* pD2GI)
	: D2GIBufferContainer<INDEX_STREAMING_BUFFER>(pD2GI)
{

}


D2GIIndexBufferContainer::~D2GIIndexBufferContainer()
{

}


INDEX_STREAMING_BUFFER* D2GIIndexBufferContainer::AllocNewBuffer(UINT uRequiredSize)
{
	UINT uSelectedSize = std::max(uRequiredSize, DEFAULT_IB_SIZE);
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::IDirect3DIndexBuffer9* pIB;

	if (FAILED(pDev->CreateIndexBuffer(uSelectedSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3D9::D3DFMT_INDEX16, D3D9::D3DPOOL_DEFAULT, &pIB, NULL)))
		Logger::Error(TEXT("Failed to alloc new index buffer for streaming"));

	push_back(INDEX_STREAMING_BUFFER(pIB, uSelectedSize));
	pIB->Release();

	return data() + size() - 1;
}


UINT D2GIIndexBufferContainer::SetAsSource()
{
	GetD3D9Device()->SetIndices(m_pLastLockBuffer->pBuffer);

	return m_uLastLockOffset / sizeof(UINT16);
}
