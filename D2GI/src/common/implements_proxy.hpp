#pragma once

#include <combaseapi.h>
#include <utility>

// A class setting up the COM interface implementation (AddRef/Release/QueryInterface) for D2GI's Proxy classes
template<typename... Proxies>
class ImplementsProxy : public Proxies...
{
public:
	virtual ~ImplementsProxy() = default;

	IFACEMETHOD_(ULONG, AddRef)() override final
	{
		return _InterlockedIncrement(&m_refCount);
	}

	IFACEMETHOD_(ULONG, Release)() override final
	{
		const LONG ref = _InterlockedDecrement(&m_refCount);
		if (ref == 0)
		{
			m_refCount = LONG_MAX/2; // Protect against a double-release
			delete this;
		}
		return ref;
	}

	IFACEMETHOD(QueryInterface)(REFIID riid, void** ppvObject) override final
	{
		if (ppvObject == nullptr) return E_POINTER;

		if (IsEqualIID(riid, IID_IUnknown))
		{
			// IUnknown must always resolve to the same interface - here, we pick the first interface from Proxies
			*ppvObject = static_cast<IUnknown*>(static_cast<std::tuple_element_t<0, std::tuple<Proxies...>>*>(this));
			AddRef();
			return S_OK;
		}

		const bool bMatched = (TryQueryInterface<Proxies>(riid, ppvObject) || ...);
		if (bMatched)
		{
			AddRef();
			return S_OK;
		}

		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

private:
	template<typename Proxy>
	bool TryQueryInterface(REFIID riid, void** ppvObject)
	{
		if (IsEqualIID(riid, Proxy::proxy_guid))
		{
			*ppvObject = static_cast<typename Proxy::proxy_type*>(this);
			return true;
		}
		return false;
	}

private:
	LONG m_refCount = 1;
};
