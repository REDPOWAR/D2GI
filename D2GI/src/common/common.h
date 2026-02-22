#pragma once

#define NOMINMAX
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#define T TEXT


#ifndef D2GI_VERSION
#define D2GI_VERSION T("UNDEFINED")
#endif

#ifndef D2GI_VERSION_STRING
#define D2GI_VERSION_STRING D2GI_VERSION
#endif

#ifndef D2GI_REPOSITORY_URL
#define D2GI_REPOSITORY_URL T("UNDEFINED")
#endif


#ifdef UNICODE
#define ASCII_STR TEXT("%S")
#else
#define ASCII_STR TEXT("%s")
#endif


#define RELEASE(x) {if((x) != NULL) {(x)->Release(); (x) = NULL;} }
#define DEL(x)     {if((x) != NULL) {delete (x); (x) = NULL;}}

