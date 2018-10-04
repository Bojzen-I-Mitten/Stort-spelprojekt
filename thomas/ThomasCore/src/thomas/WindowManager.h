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
		void UpdateFocus();
	public:
		int GetNumOfWindows();
		Window* GetCurrentBound();
		Input* GetCurrentInput();
		Input* GetGameInput();
		void SetCurrentBound(Window* value);
		EditorWindow* GetEditorWindow();
		Window* GetWindow(int index);
		Window* GetWindow(HWND hWnd);

	public:
		static WindowManager* Instance();

	private:
		WindowManager() {m_dummyInput = Input();}

	private:
		std::vector<Window*> m_windows;
		EditorWindow* m_editorWindow;
		Window* m_current;
		Input m_dummyInput;
		
	private:
		static WindowManager s_windowManager;
		
	};
}
