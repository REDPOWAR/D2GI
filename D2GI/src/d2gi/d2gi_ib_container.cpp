
#include "../common/logger.h"
#include "d2gi_ib_container.h"

#include <algorithm>

static constexpr UINT DEFAULT_IB_SIZE = 2u * 256u * 256u;

void D2GIIndexBufferContainer::AllocNewBuffer(UINT uRequiredSize)
{
	uRequiredSize = std::max(uRequiredSize, DEFAULT_IB_SIZE);
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	if (FAILED(pDev->CreateIndexBuffer(uRequiredSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		D3D9::D3DFMT_INDEX16, D3D9::D3DPOOL_DEFAULT, &m_pBuffer, nullptr)))
		Logger::Error(TEXT("Failed to alloc new index buffer for streaming"));

	m_UsedSpace = 0;
	m_TotalSpace = uRequiredSize;
}


UINT D2GIIndexBufferContainer::SetAsSource(const LockData& Data)
{
	GetD3D9Device()->SetIndices(m_pBuffer.Get());

	return Data.Offset / sizeof(UINT16);
}
