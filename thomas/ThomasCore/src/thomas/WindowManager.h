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
		void BeginFrame();
		void EndFrame();
		void ResolveRenderTarget();
		void Update();
		void Create(HWND& hwnd, HWND parent, int width, int height, bool isEditor);
		void Create(HWND& hwnd, HWND parent, int width, int height, std::string name);
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
		EditorWindow* m_editorWindow = nullptr;
		Window* m_current;
		Input m_dummyInput;
		
	private:
		static WindowManager s_windowManager;
		
	};
}
