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
	class ThomasCore
	{
	public:	
		static bool Init();
		static bool Initialized();
		static void Update();
		static bool Destroy();
		static void Exit();


	public:
		static std::vector<std::string> GetLogOutput();

	public:
		static void LogOutput(const std::string & message);
		static void ClearLogOutput();

	private:
		static bool InitDirectX();

	private:
		static bool s_initialized;
		static std::vector<std::string> s_logOutput;
		static bool s_clearLog;

	private:
		static ImGuiContext* s_imGuiContext;
	};
}