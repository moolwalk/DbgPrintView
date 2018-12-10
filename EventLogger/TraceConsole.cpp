#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include "Trace.h"

#define DEBUG_MSG_BUF_SIZE	1024

//ULONG DefaultDebugPrintLevel = TRACE_LEVEL_WARNING;
ULONG DefaultDebugPrintLevel = TRACE_LEVEL_VERBOSE;

void TraceA(ULONG tracelevel, char *formatStr, ...)
{
	char buf[DEBUG_MSG_BUF_SIZE];
	int formatStrLen;

	va_list argptr;
	va_start(argptr, formatStr);

	memset(buf, 0, sizeof(buf));
	formatStrLen = sprintf_s(buf, sizeof(buf)/sizeof(char), formatStr, argptr);

	if (formatStrLen > 0 && tracelevel <= DefaultDebugPrintLevel)
	{
		// This will output to WinDbg if the bit3 of Debug Print Filter is set (DPFLTR_INFO_LEVEL):
		// reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter" /v DEFAULT /t REG_DWORD /d 0x8
		printf(buf);
	}
}

void Trace(ULONG tracelevel, const wchar_t *formatStr, ...)
{
	TCHAR buf[DEBUG_MSG_BUF_SIZE];
	int formatStrLen;

	va_list argptr;
	va_start(argptr, formatStr);

	memset(buf, 0, sizeof(buf));
	formatStrLen = _vsntprintf_s(buf, sizeof(buf) / sizeof(TCHAR), _TRUNCATE, formatStr, argptr);

	if (formatStrLen > 0 && tracelevel <= DefaultDebugPrintLevel)
	{
		// This will output to WinDbg if the bit3 of Debug Print Filter is set (DPFLTR_INFO_LEVEL):
		// reg add "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Debug Print Filter" /v DEFAULT /t REG_DWORD /d 0x8
		_tprintf(buf);
	}
}

void SetVerboseLogLevel()
{
	DefaultDebugPrintLevel = TRACE_LEVEL_VERBOSE;
}