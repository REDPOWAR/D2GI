#pragma once

#include "d2gi_buffer_container.h"


class D2GIVertexBufferContainer : public D2GIBufferContainer<D2GIVertexBufferContainer, D3D9::IDirect3DVertexBuffer9>
{
public:
	using D2GIBufferContainer::D2GIBufferContainer;

	void SetAsSource(const LockData& Data, UINT);
	void AllocNewBuffer(UINT uSize);
};
