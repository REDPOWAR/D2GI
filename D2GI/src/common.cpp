
#include <stdarg.h>

#include "common.h"


VOID Debug(CONST TCHAR* pszFmt, ...)
{
	TCHAR szBuf[1024];
	TCHAR szOutBuf[1024];
	va_list lst;

	va_start(lst, pszFmt);
	_vstprintf(szBuf, pszFmt, lst);
	va_end(lst);

	_stprintf(szOutBuf, TEXT("[DEBUG] %s\n"), szBuf);

	OutputDebugString(szOutBuf);
}
