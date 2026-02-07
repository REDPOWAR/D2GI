#pragma once

#include "../common/common.h"

#include "d2gi_common.h"

#include <utility>
#include <wrl/client.h>

template<typename ContainerType, typename BufferT>
class D2GIBufferContainer : public D2GIBase
{
protected:
	using BufferType = BufferT;

	Microsoft::WRL::ComPtr<BufferType> m_pBuffer;
	UINT m_TotalSpace = 0, m_UsedSpace = 0;

public:
	struct LockData
	{
		void* Buffer;
		UINT Offset;

		explicit operator bool() const noexcept { return Buffer != nullptr; }
	};

	using D2GIBase::D2GIBase;

	void ReleaseResource(bool bResettingDevice)
	{
		m_pBuffer.Reset();
		m_TotalSpace = m_UsedSpace = 0;
	}

	void LoadResource(bool bResettingDevice)
	{
		static_cast<ContainerType*>(this)->AllocNewBuffer(0);
	}

	LockData LockStreamingSpace(UINT uSize)
	{
		if (uSize > m_TotalSpace)
			static_cast<ContainerType*>(this)->AllocNewBuffer(uSize);

		return Lock(uSize);
	}

	void UnlockStreamingSpace()
	{
		m_pBuffer->Unlock();
	}

protected:
	LockData Lock(UINT uSize)
	{
		DWORD Flags = D3DLOCK_NOOVERWRITE|D3DLOCK_NOSYSLOCK;
		if (m_UsedSpace + uSize > m_TotalSpace)
		{
			Flags = D3DLOCK_DISCARD|D3DLOCK_NOSYSLOCK;
			m_UsedSpace = 0;
		}

		LockData data;
		m_pBuffer->Lock(m_UsedSpace, uSize, &data.Buffer, Flags);
		data.Offset = std::exchange(m_UsedSpace, m_UsedSpace + uSize);
		return data;
	}

	void Unlock() { m_pBuffer->Unlock(); }
};
