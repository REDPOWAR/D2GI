
#include "dir.h"

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


TCHAR Directory::s_szSysDir[MAX_PATH] = { '\0' };
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


const TCHAR* Directory::GetSysDirectory()
{
	if (*s_szSysDir == '\0')
		GetSystemDirectory(s_szSysDir, MAX_PATH);

	return s_szSysDir;
}
