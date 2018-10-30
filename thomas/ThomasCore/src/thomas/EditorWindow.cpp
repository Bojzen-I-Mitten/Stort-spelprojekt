#include "EditorWindow.h"
#include "Input.h"
#include "Common.h"
#include "utils\D3D.h"
#include <imgui\imgui_impl_dx11.h>
#include <imgui\ImGuizmo.h>

namespace thomas
{
	EditorWindow::EditorWindow(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title) :
		Window(hInstance, nCmdShow, width, height, title)
	{
	}

	EditorWindow::EditorWindow(HWND hWnd) : Window(hWnd)
	{
		ImGui_ImplDX11_Init(hWnd, utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext());
	}

	EditorWindow::~EditorWindow()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void EditorWindow::Present()
	{
		Bind();

		if (ImGui_ImplDx11_Valid() && this->m_guiData)
			ImGui_ImplDX11_RenderDrawData(this->m_guiData);

		unsigned int sub = D3D11CalcSubresource(0, 0, 1);
		utils::D3D::Instance()->GetDeviceContext()->ResolveSubresource(m_dxBuffers.backbuffer, sub, m_dxBuffers.buffer[0], sub, DXGI_FORMAT_R8G8B8A8_UNORM);

		//utils::D3D::Instance()->GetDeviceContext()->CopyResource(m_dxBuffers.backbuffer, m_dxBuffers.buffer[1]);

		m_swapChain->Present(0, 0);
	}

	void EditorWindow::BeginFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGuizmo::BeginFrame();
	}

	void EditorWindow::EndFrame(bool copyGui)
	{
		if (copyGui)
		{
			ImGui::Render();
			DeleteGUIData();
			CloneGUIData();
		}
		else
			ImGui::EndFrame();
	}

	void EditorWindow::UpdateWindow()
	{
		if (!m_initialized)
		{
			InitDxBuffers();
			m_initialized = true;
		}
		if (m_shouldResize)
		{
			ImGui_ImplDX11_InvalidateDeviceObjects();
			m_initialized = false;
			Resize();
			m_initialized = true;
			m_shouldResize = false;
			ImGui_ImplDX11_CreateDeviceObjects();
		}
		BeginFrame();

	}
	void EditorWindow::CloneGUIData()
	{
		ImDrawData* data = ImGui::GetDrawData();
		ImDrawData* dst = new ImDrawData();
		dst->Valid = data->Valid;
		dst->CmdListsCount = data->CmdListsCount;
		dst->TotalIdxCount = data->TotalIdxCount;
		dst->TotalVtxCount = data->TotalVtxCount;

		dst->CmdLists = (ImDrawList**)malloc(data->CmdListsCount * sizeof(ImDrawList*));
		for (int i = 0; i < data->CmdListsCount; ++i)
			dst->CmdLists[i] = data->CmdLists[i]->CloneOutput();

		m_guiData = dst;
	}

	void EditorWindow::DeleteGUIData()
	{
		if (m_guiData)
		{
			for (int i = 0; i < m_guiData->CmdListsCount; ++i)
				delete m_guiData->CmdLists[i];

			m_guiData->Clear();
			delete m_guiData;
			m_guiData = nullptr;
		}
	}
}