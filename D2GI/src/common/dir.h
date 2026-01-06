#pragma once


#include "common.h"


class Directory
{
	static TCHAR s_szEXEDir[MAX_PATH];
public:
	static const TCHAR* GetEXEDirectory();
};
