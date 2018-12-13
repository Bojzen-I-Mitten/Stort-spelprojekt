#include "EditorWindow.h"
#include "Input.h"
#include "Common.h"
#include "utils\D3D.h"
#include "utils\GpuProfiler.h"
#include "utils\ProfileManager.h"
#include <imgui\imgui_impl_dx11.h>
#include <imgui\ImGuizmo.h>

namespace thomas
{
	EditorWindow::EditorWindow(HWND& hwnd, HWND parent, LONG width, LONG height, std::string name) :
		Window(hwnd, parent, width, height, name)
	{
		ImGui_ImplDX11_Init(hwnd, utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContextDeferred());
	}

	EditorWindow::~EditorWindow()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void EditorWindow::Present()
	{
		m_swapChain->Present(0, 0);
	}

	void EditorWindow::BeginFrame()
	{
		utils::D3D::Instance()->ResetCommandList(m_dx.commandList);
		Clear();
	}

	void EditorWindow::EndFrame()
	{
		BindBackBuffer();

		if (ImGui_ImplDx11_Valid() && m_guiData)
			ImGui_ImplDX11_RenderDrawData(m_guiData);

		utils::D3D::Instance()->FinishCommandList(m_dx.commandList);
		utils::D3D::Instance()->ExecuteCommandList(m_dx.commandList);
		Present();
	}

	void EditorWindow::RenderGUIData()
	{
		ImGui::Render();
		DeleteGUIData();
		CloneGUIData();
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
			utils::profiling::GpuProfiler::Instance()->GetMemoryUsage();
			ImGui_ImplDX11_InvalidateDeviceObjects();
			m_initialized = false;
			Resize();
			m_initialized = true;
			m_shouldResize = false;
			ImGui_ImplDX11_CreateDeviceObjects();
		}

		ImGui_ImplDX11_NewFrame();
		ImGuizmo::BeginFrame();
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
			delete[] m_guiData->CmdLists;
			m_guiData->Clear();
			delete m_guiData;
			m_guiData = nullptr;
		}
	}
}