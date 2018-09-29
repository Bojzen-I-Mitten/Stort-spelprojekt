#pragma once
#include "ThomasManaged.h"
using namespace System;
namespace ThomasEngine {

	public enum class MessageSeverity
	{
		Info,
		Warning, 
		Error,
		ThomasCore
	};
	public ref class DebugMessage {
	public:
		property String^ Message;
		property int Count;
		MessageSeverity Severity;
		property array<String^>^ StackTrace;
		DebugMessage(String^ msg, MessageSeverity severity) {
			this->Count = 1;
			this->Message = msg;
			this->Severity = severity;
			if (this->Severity == MessageSeverity::ThomasCore)
				this->StackTrace = gcnew array<String^>{"ThomasCore"};
			else
				this->StackTrace = GetStack(6);
		}

	private:
		array<String^>^ GetStack(int removeLines)
		{
			array<String^>^ stack = Environment::StackTrace->Split(
				gcnew array<String^>{ Environment::NewLine },
				StringSplitOptions::RemoveEmptyEntries);

			if (stack->Length <= removeLines)
				return gcnew array<String^>(0);

			array<String^>^ actualResult = gcnew array<String^>(stack->Length - removeLines);
			for (int i = removeLines; i < stack->Length; i++) {
				// Remove 6 characters (e.g. "  at ") from the beginning of the line
				// This might be different for other languages and platforms
				String^ line = stack[i]->Substring(6);
				int firstSlash = line->IndexOf(":\\");
				int lastSlash = line->LastIndexOf("\\");
				if(firstSlash > 0 && lastSlash > 0)
					line = line->Remove(firstSlash-1, (lastSlash+2) - firstSlash);
				actualResult[i - removeLines] = line;
			}
				
				

			return actualResult;
		}

	};
	public ref class Debug {
	private:
		static void Log(String^ message, MessageSeverity severity)
		{
			DebugMessage^ newMessage = gcnew DebugMessage(message, severity);
			OnDebugMessage(newMessage);
		}
		static void LogCore(String^ message)
		{
			Log(message, MessageSeverity::ThomasCore);
		}
	public:
		delegate void DebugMessageDelegate(DebugMessage^ newMessage);
		static event DebugMessageDelegate^ OnDebugMessage;

		static void Log(Object^ message) {
			Log(message->ToString(), MessageSeverity::Info);
		}
		static void LogWarning(Object^ message) {
			Log(message->ToString(), MessageSeverity::Warning);
		}

		static void LogError(Object^ message) {
			Log(message->ToString(), MessageSeverity::Error);
		}
		static void LogException(Exception^ exception) {
			Log(exception->Message, MessageSeverity::Error);
		}

		

		static void UpdateCoreLog();
	};
}