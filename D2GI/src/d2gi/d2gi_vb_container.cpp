
#include "../common/logger.h"
#include "d2gi_vb_container.h"

#include <algorithm>

static constexpr UINT DEFAULT_VB_SIZE = 32u * 256u * 256u;

void D2GIVertexBufferContainer::AllocNewBuffer(UINT uRequiredSize)
{
	uRequiredSize = std::max(uRequiredSize, DEFAULT_VB_SIZE);
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();

	if (FAILED(pDev->CreateVertexBuffer(uRequiredSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY,
		0, D3D9::D3DPOOL_DEFAULT, &m_pBuffer, nullptr)))
		Logger::Error(TEXT("Failed to alloc new vertex buffer for streaming"));


	m_UsedSpace = 0;
	m_TotalSpace = uRequiredSize;
}


void D2GIVertexBufferContainer::SetAsSource(const LockData& Data, UINT uStride)
{
	GetD3D9Device()->SetStreamSource(0, m_pBuffer.Get(), Data.Offset, uStride);
}
