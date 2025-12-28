
#include "dir.h"

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


TCHAR Directory::s_szEXEDir[MAX_PATH] = { '\0' };


const TCHAR* Directory::GetEXEDirectory()
{
	if (*s_szEXEDir == '\0')
	{
		GetModuleFileName(NULL, s_szEXEDir, MAX_PATH);
		PathRemoveFileSpec(s_szEXEDir);
	}

	return s_szEXEDir;
}
