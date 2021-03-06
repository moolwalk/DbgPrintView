#include "stdafx.h"

#include "EventLogger.h"

#define INITGUID
#include <tdh.h>
#include <stdio.h>
#include <tchar.h>
#include <vcclr.h >
#include <string>
#include <msclr\marshal.h>
#include "Trace.h"

using namespace System;
using namespace Runtime::InteropServices;  
using namespace EventLogger;
using namespace std;
using namespace msclr::interop;

DEFINE_GUID( /* 13976D09-A327-438c-950B-7F03192815C7  */
	DbgPrintGuid,
	0x13976d09,
	0xa327,
	0x438c,
	0x95, 0xb, 0x7f, 0x3, 0x19, 0x28, 0x15, 0xc7
);

DEFINE_GUID( /* A676748F-653F-2F0E-C855-2377E4ADB53B  */
	OutputDebugGuid,
	0xA676748F,
	0x653F,
	0x2F0E,
	0xC8, 0x55, 0x23, 0x77, 0xE4, 0xAD, 0xB5, 0x3B
);


typedef struct CONVERSION_STATS {
	ULONGLONG   LogCount;
	ULONGLONG   EventCount;
	ULONGLONG   TotalCount;
	FILE *      OutputFile;
} CONVERSION_STATS, *PCONVERSION_STATS;

extern "C" DWORD WINAPI EventLogger_MainThread(LPVOID lpThreadParameter);

std::wstring m_LogString;
TCHAR m_Buffer[1024];

Guid FromGUID(_GUID& guid) {
	return Guid(guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);
}

VOID
WINAPI
EventRecordCallback(
	__in PEVENT_RECORD Event
)
{
	ULONGLONG TimeStamp = 0;
	ULONGLONG Nanoseconds = 0;
	SYSTEMTIME st;
	SYSTEMTIME stLocal;
	::FILETIME ft;
	char *str;

	PCONVERSION_STATS stats = (PCONVERSION_STATS)Event->UserContext;
	//    if ((Event->EventHeader.ProviderId == DbgPrintGuid || Event->EventHeader.ProviderId == OutputDebugGuid) &&
	if (Event->EventHeader.EventDescriptor.Id <= MAXBYTE)
	{
		if (Event->EventHeader.Flags)
		{
			// Print the time stamp for when the event occurred.
			ft.dwHighDateTime = Event->EventHeader.TimeStamp.HighPart;
			ft.dwLowDateTime = Event->EventHeader.TimeStamp.LowPart;

			FileTimeToSystemTime(&ft, &st);
			SystemTimeToTzSpecificLocalTime(NULL, &st, &stLocal);

			TimeStamp = Event->EventHeader.TimeStamp.QuadPart;
			Nanoseconds = (TimeStamp % 10000000) * 100;

			_stprintf_s(m_Buffer, _T("%02d/%02d/%02d %02d:%02d:%02d.%I64u "),
				stLocal.wMonth, stLocal.wDay, stLocal.wYear, stLocal.wHour, stLocal.wMinute, stLocal.wSecond, Nanoseconds);
#if SHOW_PROVIDER_GUID
			fprintf(stats->OutputFile, "%x ", Event->EventHeader.ProviderId);
#endif

			if (NULL != Event->UserData)
			{
				// Write the event data to the file, excluding the first two
				// bytes (the length of the raw Diag packet that follows).
				if (Event->EventHeader.ProviderId == DbgPrintGuid)
				{
					str = (char *)Event->UserData + 8;

					_stprintf_s(m_Buffer, _T("%hs"), str);

					//
					// Add to the LogString globals.
					//
					m_LogString = m_Buffer;
					Trace(TRACE_LEVEL_INFORMATION, _T("%s"), m_Buffer);
					EventLoggerData^ eventData;
					eventData = gcnew EventLoggerData();
					eventData->ProviderId = FromGUID(Event->EventHeader.ProviderId);
					if (CEventLogger::m_EventCallback)
					{
						CEventLogger::m_EventCallback->OnEventLoggerCallback(eventData);
					}
					stats->LogCount++;
				}
				else
				{
					str = (char *)Event->UserData;
				}

			}
			else
			{
				printf_s("No user data\n");
			}

		}
	}

	stats->TotalCount++;
}

WCHAR *CEventLogger::ConvertToWideCharArray(String ^managedString)
{
	// Pin memory so GC can't move it while native function is called
	pin_ptr<const wchar_t> wch = PtrToStringChars(managedString);
	size_t convertedChars = 0;
	size_t  sizeInBytes = ((managedString->Length + 1) * 2);
	errno_t err = 0;
	WCHAR    *ch = (WCHAR *)malloc(sizeInBytes*2);
	err = wcscpy_s(ch, sizeInBytes, wch);

	return ch;
}

CEventLogger::CEventLogger(String ^loggerName)
{
	m_LoggerName = ConvertToWideCharArray(loggerName);
}

ULONG CEventLogger::Loop()
{
	ULONG status = ERROR_SUCCESS;
	EVENT_TRACE_LOGFILE etlFile = {};
	CONVERSION_STATS stats = {};
	TRACEHANDLE hTrace = NULL;
	//errno_t err;

	etlFile.LoggerName = m_LoggerName;
	etlFile.ProcessTraceMode = PROCESS_TRACE_MODE_EVENT_RECORD | PROCESS_TRACE_MODE_REAL_TIME;
	etlFile.EventRecordCallback = EventRecordCallback;
	etlFile.Context = (PVOID)&stats;

	hTrace = OpenTrace(&etlFile);
	if (INVALID_PROCESSTRACE_HANDLE == hTrace)
	{
		status = GetLastError();
		Trace(TRACE_LEVEL_ERROR, _T("OpenTrace failed with error code: %lu\n"), status);
		goto failexit;
	}

	status = ProcessTrace(&hTrace, 1, NULL, NULL);
	if (ERROR_SUCCESS != status)
	{
		Trace(TRACE_LEVEL_ERROR, _T("ProcessTrace failed with error code: %lu\n"), status);
		goto failexit;
	}

	Trace(TRACE_LEVEL_VERBOSE, _T("\nConversion Report:"));
	Trace(TRACE_LEVEL_VERBOSE, _T("\n------------------\n"));
	Trace(TRACE_LEVEL_VERBOSE, _T(" * Logs:     %I64u\n"), stats.LogCount);
	Trace(TRACE_LEVEL_VERBOSE, _T(" * Total Events:   %I64u\n\n"), stats.TotalCount);

failexit:
	CloseTrace(hTrace);
	return status;
}

String ^CEventLogger::GetLogString()
{
	String^ retStr;
	retStr = marshal_as<String^>(m_Buffer);
	return retStr;
}

void CEventLogger::SetEventCallback(IEventCallback ^callback)
{
	m_EventCallback = callback;
}

void CEventLogger::Destroy()
{
	
}

