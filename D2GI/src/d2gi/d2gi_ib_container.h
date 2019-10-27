#pragma once

#include "d2gi_buffer_container.h"


class D2GIIndexBufferContainer : public D2GIBufferContainer<INDEX_STREAMING_BUFFER>
{
protected:
	virtual INDEX_STREAMING_BUFFER* AllocNewBuffer(UINT);
public:
	D2GIIndexBufferContainer(D2GI*);
	~D2GIIndexBufferContainer();

	UINT SetAsSource();
};
