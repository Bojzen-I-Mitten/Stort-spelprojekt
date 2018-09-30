#pragma once

#include "Window.h"
#include <imgui\imgui.h>

namespace thomas
{
	class EditorWindow : public Window
	{
	public:
		~EditorWindow();

	public:
		void Present();
		static void BeginFrame();
		static void EndFrame(bool copyGui);
		static EditorWindow* Create(HWND hWnd);

	public:
		static EditorWindow* GetWindow();

	private:
		EditorWindow(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title);
		EditorWindow(HWND hWnd);

	private:
		void UpdateWindow();
		void CloneGUIData();
		void DeleteGUIData();

	private:
		ImDrawData* m_guiData = nullptr;

	private:
		static EditorWindow* s_editorWindow;
	};
}
