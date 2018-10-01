#pragma once
#include "Window.h"
#include "EditorWindow.h"

namespace thomas
{
	class WindowManager
	{
	public:
		~WindowManager() = default;

	public:
		bool WaitingForUpdate();
		void Destroy();
		void ClearAllWindows();
		void PresentAllWindows();
		void Update();
		void Create(HWND hWnd, bool isEditor);

	public:
		int GetNumOfWindows();
		EditorWindow* GetEditorWindow();
		Window* GetWindow(int index);
		Window* GetWindow(HWND hWnd);

	public:
		Window* s_focused;
		static WindowManager* Instance();

	private:
		WindowManager() = default;

	private:
		std::vector<Window*> m_windows;
		EditorWindow* m_editorWindow;

	private:
		static WindowManager s_windowManager;
	};
}
