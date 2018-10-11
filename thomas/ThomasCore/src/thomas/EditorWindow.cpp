#include "EditorWindow.h"
#include "Input.h"
#include "Common.h"
#include "utils\D3D.h"
#include <imgui\imgui_impl_dx11.h>
#include <imgui\ImGuizmo.h>
#include <include/DirectXTK/WICTextureLoader.h>

namespace thomas
{
	EditorWindow::EditorWindow(HINSTANCE hInstance, int nCmdShow, const LONG & width, const LONG & height, const LPCSTR & title) :
		Window(hInstance, nCmdShow, width, height, title)
	{
	}

	EditorWindow::EditorWindow(HWND hWnd) : Window(hWnd)
	{
		m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());

		ID3D11Resource* resource;
		HRESULT hr;

		CoInitialize(NULL);
		hr= DirectX::CreateWICTextureFromFile(utils::D3D::Instance()->GetDevice(), 
				L"../Data/cat.png", &resource, &m_texture);
		CoUninitialize();
		
		ID3D11Texture2D* cat;
		cat = (ID3D11Texture2D*)resource;

		CD3D11_TEXTURE2D_DESC catDesc;
		cat->GetDesc(&catDesc);

		m_origin.x = float(catDesc.Width / 2);
		m_origin.y = float(catDesc.Height / 2);
		m_screenPos.x = 300 / 2.f;
		m_screenPos.y = 300 / 2.f;

		//cat->Release();
		//resource->Release();

		ImGui_ImplDX11_Init(hWnd, utils::D3D::Instance()->GetDevice(), utils::D3D::Instance()->GetDeviceContext());
	}

	EditorWindow::~EditorWindow()
	{
		m_spriteBatch.reset();
		ImGui_ImplDX11_Shutdown();
	}

	void EditorWindow::Present()
	{
		this->Bind();

		if (ImGui_ImplDx11_Valid() && this->m_guiData)
			ImGui_ImplDX11_RenderDrawData(this->m_guiData);

		m_spriteBatch->Begin();

		m_spriteBatch->Draw(m_texture, m_screenPos, nullptr, Colors::White,
			0.f, m_origin);

		m_spriteBatch->Draw(m_texture, Vector2(m_screenPos.x + 50.f, m_screenPos.y), nullptr, Colors::White,
			0.f, m_origin);

		m_spriteBatch->End();
	
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