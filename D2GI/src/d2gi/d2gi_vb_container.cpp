
#include "d2gi_vb_container.h"


#define DEFAULT_VB_SIZE  (32 * 256 * 256)


D2GIVertexBufferContainer::D2GIVertexBufferContainer(D2GI* pD2GI) 
	: D2GIBufferContainer<VERTEX_STREAMING_BUFFER>(pD2GI)
{

}


D2GIVertexBufferContainer::~D2GIVertexBufferContainer()
{

}


VERTEX_STREAMING_BUFFER* D2GIVertexBufferContainer::AllocNewBuffer(UINT uRequiredSize)
{
	UINT uSelectedSize = max(uRequiredSize, DEFAULT_VB_SIZE);
	D3D9::IDirect3DDevice9* pDev = GetD3D9Device();
	D3D9::IDirect3DVertexBuffer9* pVB;

	pDev->CreateVertexBuffer(uSelectedSize, D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, 0, D3D9::D3DPOOL_DEFAULT, &pVB, NULL);
	push_back(VERTEX_STREAMING_BUFFER(pVB, uSelectedSize));
	pVB->Release();

	return data() + size() - 1;
}


VOID D2GIVertexBufferContainer::SetAsSource(UINT uStride)
{
	GetD3D9Device()->SetStreamSource(0, m_pLastLockBuffer->pBuffer, m_uLastLockOffset, uStride);
}
