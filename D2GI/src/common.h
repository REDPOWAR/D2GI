#pragma once

#include <windows.h>
#include <tchar.h>


#define RELEASE(x) {if((x) != NULL) {(x)->Release(); (x) = NULL;} }
#define DEL(x)     {if((x) != NULL) {delete (x); (x) = NULL;}}


VOID Debug(CONST TCHAR*, ...);
