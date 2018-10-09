#include "EditorWindow.h"
#include "Input.h"
#include "Common.h"
#include "utils\D3D.h"
#include <imgui\imgui_impl_dx11.h>
#include <imgui\ImGuizmo.h>

// Nuklear
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D11_IMPLEMENTATION
#include <nuklear/nuklear.h>
#include <nuklear/nuklear_d3d11.h>

struct nk_context* m_nkContext;

namespace thomas
{
	EditorWindow::EditorWindow(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title) : 
		Window(hInstance, nCmdShow, width, height, title)
	{
	}

	EditorWindow::EditorWindow(HWND hWnd) : Window(hWnd)
	{
		m_nkContext = nk_d3d11_init(utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext(), 
									0, 0, MAX_VERTEX_BUFFER, MAX_INDEX_BUFFER);

		struct nk_font_atlas* atlas;
		nk_d3d11_font_stash_begin(&atlas);
		nk_d3d11_font_stash_end();

		ImGui_ImplDX11_Init(hWnd, utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext());
	}

	EditorWindow::~EditorWindow()
	{
		ImGui_ImplDX11_Shutdown();
	}

	void EditorWindow::Present()
	{
		this->Bind();
		if (ImGui_ImplDx11_Valid() && this->m_guiData)
			ImGui_ImplDX11_RenderDrawData(this->m_guiData);

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