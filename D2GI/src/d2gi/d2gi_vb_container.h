#pragma once

#include "d2gi_buffer_container.h"


class D2GIVertexBufferContainer : public D2GIBufferContainer<VERTEX_STREAMING_BUFFER>
{
protected:
	virtual VERTEX_STREAMING_BUFFER* AllocNewBuffer(UINT);
public:
	D2GIVertexBufferContainer(D2GI*);
	~D2GIVertexBufferContainer();

	VOID SetAsSource(UINT);
};
