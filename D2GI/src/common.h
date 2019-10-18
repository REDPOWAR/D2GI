#pragma once

#include <windows.h>
#include <tchar.h>


#define RELEASE(x) {if((x) != NULL) {(x)->Release(); (x) = NULL;} }
#define DEL(x)     {if((x) != NULL) {delete (x); (x) = NULL;}}


class Unknown : public IUnknown
{
protected:
	UINT m_uRefCount;
public:
	Unknown() : m_uRefCount(1) {}
	virtual ~Unknown() {}

	STDMETHOD_(ULONG, AddRef)() { return ++m_uRefCount; }
	STDMETHOD_(ULONG, Release)() { ULONG uRes = --m_uRefCount; if (uRes == 0) delete this; return uRes; }
	STDMETHOD(QueryInterface)(REFIID, LPVOID FAR*) { return S_FALSE; }
};


VOID Debug(CONST TCHAR*, ...);
