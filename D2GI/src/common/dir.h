#pragma once


#include "common.h"


class Directory
{
	static TCHAR s_szSysDir[MAX_PATH];
	static TCHAR s_szEXEDir[MAX_PATH];
public:
	static TCHAR* GetEXEDirectory();
	static TCHAR* GetSysDirectory();
};
