/*
	Assembly class of the engine (creates, updates and destroys objects).
*/

#pragma once
#pragma unmanaged	// Require for: std::thread
#include <d3d11.h>
#include <vector>
#include <string>
#include <imgui\imgui.h>
#include <thread>

namespace thomas
{

	namespace resource {
		class MemoryAllocation;
	}

	class ThomasCore
	{
	public:	
		static bool Init();
		static bool Initialized();
		static void Update();
		static void Render();
		static bool Destroy();
		static void Exit();

		ThomasCore();

	public:
		static std::vector<std::string> GetLogOutput();

		static ThomasCore& Core();

		/* Register executing thread (should be called by all threads using core functions).
		*/
		void registerThread();
		/* Get index of executing thread. Should preferably 'not' be used in non-debug code.
		*/
		uint32_t Thread_Index();

		resource::MemoryAllocation* Memory();

	public:
		static void LogOutput(const std::string & message);
		static void ClearLogOutput();
		static bool IsEditor();
		static void SetEditor(bool value);
	private:
		static bool InitDirectX();

	private:

		
		std::vector<std::thread::id> m_thread_tracker;
		resource::MemoryAllocation* m_memAlloc;

		static bool s_initialized;
		static std::vector<std::string> s_logOutput;
		static bool s_clearLog;
		static bool s_isEditor;

	private:
		static ImGuiContext* s_imGuiContext;
	};
}