/*
	Assembly class of the engine (creates, updates and destroys objects).
*/

#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <imgui\imgui.h>

namespace thomas
{

	namespace resource {
		class MemoryAllocation;
	}
	namespace utils {
		class ThreadMap;
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

		/* Thread map used to register threads.
		*/
		utils::ThreadMap& getThreadMap();
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

		std::unique_ptr<utils::ThreadMap> m_threadMap;
		resource::MemoryAllocation* m_memAlloc;

		static bool s_initialized;
		static std::vector<std::string> s_logOutput;
		static bool s_clearLog;
		static bool s_isEditor;

	private:
		static ImGuiContext* s_imGuiContext;
	};
}