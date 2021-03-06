#pragma once

using namespace System;

namespace EventLogger {

	// 
	// Data structure to deliver the EventLogData data to OnSensorEvent() callback method(C#).
	// 
	public ref class EventLoggerData
	{
	public:
		Guid ProviderId;
	};

	//
	// Event Callback interface is the callback methods that is called when
	// there's an event notification from EventLogger.dll.
	//
	// IEventCallback must be implemented by C# code.
	//
	public interface class IEventCallback
	{
	public:
		virtual void OnEventLoggerCallback(EventLoggerData ^eventData);

	};

	public ref class CEventLogger sealed
	{
	public:
		static IEventCallback ^m_EventCallback = nullptr;

	public:
		WCHAR * m_LoggerName;
		CEventLogger(String ^loggerName);
		String ^GetLogString();
		ULONG Loop();
		void Destroy();

		// Set Event Callback method.
		//   C# code must implement the Event callback methods
		//   using IEventCallback interface class.
		void SetEventCallback(IEventCallback ^callback);

	private:
		WCHAR *ConvertToWideCharArray(String ^managedString);
	};
}

