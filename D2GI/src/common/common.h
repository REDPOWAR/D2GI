#pragma once

#define NOMINMAX
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

