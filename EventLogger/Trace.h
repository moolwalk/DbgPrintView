#pragma once

#if !defined(TRACE_LEVEL_ERROR)
#define TRACE_LEVEL_NONE		  0
#define TRACE_LEVEL_CRITICAL	  1
#define TRACE_LEVEL_FATAL 	  1
#define TRACE_LEVEL_ERROR 	  2
#define TRACE_LEVEL_WARNING	  3
#define TRACE_LEVEL_INFORMATION 4
#define TRACE_LEVEL_VERBOSE	  5
#define TRACE_LEVEL_RELEASE	  6 /* used to force output in release builds */
#define TRACE_LEVEL_RESERVED7   7
#define TRACE_LEVEL_RESERVED8   8
#define TRACE_LEVEL_RESERVED9   9
#endif

void TraceA(char *pszDebugMsg, ...);
VOID Trace(_In_ ULONG DebugPrintLevel, const wchar_t *pszDebugMsg, ...);
void SetVerboseLogLevel();