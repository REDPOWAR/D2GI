#pragma once

#include <windows.h>
#include <tchar.h>
#include <stdio.h>


#ifndef D2GI_VERSION
#define D2GI_VERSION TEXT("UNDEFINED")
#endif


#ifdef UNICODE
#define ASCII_STR TEXT("%S")
#else
#define ASCII_STR TEXT("%s")
#endif


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

