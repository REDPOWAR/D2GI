#pragma once

#include "d2gi_buffer_container.h"


class D2GIIndexBufferContainer : public D2GIBufferContainer<D2GIIndexBufferContainer, D3D9::IDirect3DIndexBuffer9>
{
public:
	using D2GIBufferContainer::D2GIBufferContainer;

	UINT SetAsSource(const LockData& Data);
	void AllocNewBuffer(UINT uSize);
};
