#pragma once

#include <vector>

#include "../common/common.h"

#include "d2gi_common.h"


template<typename BufferType>
struct STREAMING_BUFFER
{
	UINT uTotalSize, uCurrentlyFreeSize;
	BufferType* pBuffer;

	STREAMING_BUFFER(BufferType* pBuffer, UINT uSize)
	{
		uTotalSize = uSize; uCurrentlyFreeSize = uSize; this->pBuffer = pBuffer;
		pBuffer->AddRef();
	};

	STREAMING_BUFFER(CONST STREAMING_BUFFER& other)
	{
		uTotalSize = other.uTotalSize; uCurrentlyFreeSize = other.uCurrentlyFreeSize; pBuffer = other.pBuffer;
		pBuffer->AddRef();
	}

	STREAMING_BUFFER& operator = (CONST STREAMING_BUFFER& other)
	{
		if (&other == this) return;
		uTotalSize = other.uTotalSize; uCurrentlyFreeSize = other.uCurrentlyFreeSize; pBuffer = other.pBuffer;
		pBuffer->AddRef();
	}

	~STREAMING_BUFFER() { RELEASE(pBuffer); }

	VOID Clear() { uCurrentlyFreeSize = uTotalSize; }

	VOID* Lock(UINT uSize, UINT* pOffset)
	{
		if (uSize > uCurrentlyFreeSize)
			return NULL;

		VOID* pData;

		*pOffset = uTotalSize - uCurrentlyFreeSize;
		pBuffer->Lock(*pOffset, uSize, &pData,
			uCurrentlyFreeSize == uTotalSize ? D3DLOCK_DISCARD : D3DLOCK_NOOVERWRITE);

		uCurrentlyFreeSize -= uSize;

		return pData;
	}

	VOID Unlock() { pBuffer->Unlock(); }
};


typedef STREAMING_BUFFER<D3D9::IDirect3DVertexBuffer9> VERTEX_STREAMING_BUFFER;
typedef STREAMING_BUFFER<D3D9::IDirect3DIndexBuffer9> INDEX_STREAMING_BUFFER;


template<typename StreamingBufferType>
class D2GIBufferContainer : protected std::vector<StreamingBufferType>, public D2GIBase
{
protected:
	StreamingBufferType* m_pLastLockBuffer;
	UINT m_uLastLockOffset, m_uLastLockSize;

	virtual StreamingBufferType* AllocNewBuffer(UINT) = 0;
public:
	D2GIBufferContainer(D2GI*);
	~D2GIBufferContainer();

	VOID ReleaseResource();
	VOID LoadResource();
	VOID Clear();

	VOID* LockStreamingSpace(UINT);
	VOID UnlockStreamingSpace();
};
