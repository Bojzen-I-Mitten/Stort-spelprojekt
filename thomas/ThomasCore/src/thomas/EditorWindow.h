#pragma once
#include "Window.h"
#include <imgui/imgui.h>

namespace thomas
{
	class EditorWindow : public Window
	{
	public:
		EditorWindow(HWND& hwnd, HWND parent, LONG width, LONG height, std::string name);
		/*EditorWindow(HWND hWnd);*/
		~EditorWindow();

	public:
		void Present();
		void BeginFrame();
		void EndFrame();
		void RenderGUIData();
		virtual bool IsEditor() { return true; }

	private:
		void UpdateWindow();
		void CloneGUIData();
		void DeleteGUIData();

	private:
		ImDrawData* m_guiData = nullptr;
	};
}
