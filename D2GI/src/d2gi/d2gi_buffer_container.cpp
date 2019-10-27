
#include "d2gi_buffer_container.h"


template class D2GIBufferContainer<VERTEX_STREAMING_BUFFER>;
template class D2GIBufferContainer<INDEX_STREAMING_BUFFER>;


template<typename StreamingBufferType>
D2GIBufferContainer<StreamingBufferType>::D2GIBufferContainer(D2GI* pD2GI) 
	: D2GIBase(pD2GI)
{
}


template<typename StreamingBufferType>
D2GIBufferContainer<StreamingBufferType>::~D2GIBufferContainer()
{
	ReleaseResource();
}


template<typename StreamingBufferType>
VOID D2GIBufferContainer<StreamingBufferType>::ReleaseResource()
{
	clear();
}


template<typename StreamingBufferType>
VOID D2GIBufferContainer<StreamingBufferType>::LoadResource()
{

}


template<typename StreamingBufferType>
VOID D2GIBufferContainer<StreamingBufferType>::Clear()
{
	for (iterator it = begin(); it < end(); it++)
		it->Clear();
}


template<typename StreamingBufferType>
VOID* D2GIBufferContainer<StreamingBufferType>::LockStreamingSpace(UINT uSize)
{
	UINT i;
	StreamingBufferType* pBuffer;

	for (i = 0, pBuffer = NULL; i < size(); i++)
	{
		StreamingBufferType* pCurrent = data() + i;

		if (pCurrent->uCurrentlyFreeSize >= uSize)
		{
			pBuffer = pCurrent;
			break;
		}
	}

	if (pBuffer == NULL)
		pBuffer = AllocNewBuffer(uSize);

	VOID* pLockData;

	m_pLastLockBuffer = pBuffer;
	m_uLastLockSize = uSize;
	pLockData = pBuffer->Lock(uSize, &m_uLastLockOffset);

	return pLockData;
}


template<typename StreamingBufferType>
VOID D2GIBufferContainer<StreamingBufferType>::UnlockStreamingSpace()
{
	m_pLastLockBuffer->Unlock();
}
