#pragma once


#include "common.h"


class Directory
{
	static TCHAR s_szSysDir[MAX_PATH];
	static TCHAR s_szEXEPath[MAX_PATH], s_szEXEDir[MAX_PATH];
public:
	static TCHAR* GetEXEPath();
	static TCHAR* GetEXEDirectory();
	static TCHAR* GetSysDirectory();
};
