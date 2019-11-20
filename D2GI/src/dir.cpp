
#include "dir.h"


TCHAR Directory::s_szSysDir[MAX_PATH] = { '\0' };
TCHAR Directory::s_szEXEDir[MAX_PATH] = { '\0' };
TCHAR Directory::s_szEXEPath[MAX_PATH] = { '\0' };


TCHAR* Directory::GetEXEDirectory()
{
	if (*s_szEXEDir == '\0')
	{
		TCHAR* c;

		GetModuleFileName(NULL, s_szEXEDir, MAX_PATH);
		c = s_szEXEDir + _tcslen(s_szEXEDir);
		while (*c != '\\' && c >= s_szEXEDir)
			*(c--) = '\0';
	}

	return s_szEXEDir;
}


TCHAR* Directory::GetEXEPath()
{
	if (*s_szEXEPath == '\0')
		GetModuleFileName(NULL, s_szEXEPath, MAX_PATH);

	return s_szEXEPath;
}


TCHAR* Directory::GetSysDirectory()
{
	if (*s_szSysDir == '\0')
		GetSystemDirectory(s_szSysDir, MAX_PATH);

	return s_szSysDir;
}
