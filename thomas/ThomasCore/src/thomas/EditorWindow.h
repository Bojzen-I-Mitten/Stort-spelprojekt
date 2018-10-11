#pragma once
#include "Window.h"
#include <imgui/imgui.h>
#include <DirectXTK/SpriteBatch.h>

using namespace DirectX::SimpleMath;

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
		virtual bool IsEditor() { return true; }

	private:
		void UpdateWindow();
		void CloneGUIData();
		void DeleteGUIData();

	private:
		ID3D11ShaderResourceView* m_texture;
		std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;
		Vector2 m_screenPos;
		Vector2 m_origin;
		ImDrawData* m_guiData = nullptr;
	};
}
