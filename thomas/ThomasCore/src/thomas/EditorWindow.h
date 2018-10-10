#pragma once
#include "Window.h"
#include <imgui\imgui.h>

namespace thomas
{
	class EditorWindow : public Window
	{
	public:
		EditorWindow(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title);
		EditorWindow(HWND hWnd);
		~EditorWindow();

	public:
		void Present();
		void BeginFrame();
		void EndFrame(bool copyGui);
		void ResizeNK(int width, int height);
		int HandleEvent(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
		virtual bool IsEditor() { return true; }

	private:
		void UpdateWindow();
		void CloneGUIData();
		void DeleteGUIData();

	private:
		ImDrawData* m_guiData = nullptr;
	};
}
