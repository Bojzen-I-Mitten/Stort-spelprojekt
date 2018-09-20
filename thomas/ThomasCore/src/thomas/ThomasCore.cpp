#include "ThomasCore.h"
#include "Sound.h"
#include "Input.h"
#include "Window.h"
#include "ThomasTime.h"
#include "object/Object.h"
#include "resource/texture\Texture2D.h"
#include "resource/Shader.h"
#include "resource/Material.h"
#include "editor/EditorCamera.h"
#include "editor/gizmos\Gizmos.h"
#include "utils/Primitives.h"
#include "object/component/LightComponent.h"

namespace thomas 
{
	ID3D11Device* ThomasCore::s_device;
	ID3D11DeviceContext* ThomasCore::s_context;
	IDXGIDebug* ThomasCore::s_debug;
	IDXGIInfoQueue* ThomasCore::s_infoQueue;

	std::vector<std::string> ThomasCore::s_logOutput;
	bool ThomasCore::s_clearLog;
	bool ThomasCore::s_initialized;
	ImGuiContext* ThomasCore::s_imGuiContext;

	bool ThomasCore::Init()
	{
		s_imGuiContext = ImGui::CreateContext();
		s_logOutput.reserve(10);

		//Init all required classes
		if (!InitDirectX())
			s_initialized = false;

		Input::Init();
		resource::Texture2D::Init();
		ThomasTime::Init();
		Sound::Init();
		resource::Shader::Init();

		resource::Material::Init();
		Physics::Init();
		editor::EditorCamera::Init();
		editor::Gizmos::Init();

		graphics::LightManager::Initialize();

		s_initialized = true;
		return s_initialized;
	}

	void ThomasCore::Update()
	{
		if (s_clearLog)
		{
			s_logOutput.clear();
			s_clearLog = false;
		}

		object::Object::Clean();
		editor::EditorCamera::Update();
		resource::Shader::Update();
		Input::Update();		
		Sound::Update();
	}

	void ThomasCore::Exit()
	{
		s_initialized = false;
	}

	bool ThomasCore::Initialized()
	{
		return s_initialized;
	}

	bool ThomasCore::Destroy()
	{	
		//Destroy all objects
		Window::Destroy();
		graphics::LightManager::Destroy();
		resource::Shader::DestroyAllShaders();
		resource::Material::Destroy();
		resource::Texture2D::Destroy();
		object::Object::Destroy();
		editor::EditorCamera::Destroy();
		editor::Gizmos::Destroy();
		utils::Primitives::Destroy();
		Physics::Destroy();
		Sound::Destroy();
		ImGui::DestroyContext(s_imGuiContext);

		//Release
		s_context->ClearState();
		s_context->Flush();
		
		s_context->Release();
		s_device->Release();

		if (s_debug)
			s_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_DETAIL);

		s_debug->Release();
		s_infoQueue->Release();

		return true;
	}

	ID3D11Device * ThomasCore::GetDevice()
	{
		return s_device;
	}

	ID3D11DeviceContext* ThomasCore::GetDeviceContext()
	{
		return s_context;
	}

	std::vector<std::string> ThomasCore::GetLogOutput()
	{
		return s_logOutput;
	}

	void ThomasCore::LogOutput(const std::string & message)
	{
		s_logOutput.push_back(message);
		if (s_logOutput.size() > 10)
			s_logOutput.erase(s_logOutput.begin());
	}

	void ThomasCore::ClearLogOutput()
	{
		s_clearLog = true;
	}

	bool ThomasCore::InitDirectX()
	{
		if (!s_device)
		{
			CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
			if (!CreateDeviceAndContext())
				return false;
		}

#ifdef  _DEBUG_DX
		CreateDebugInterface();
#endif

		return true;
	}

	bool ThomasCore::CreateDeviceAndContext()
	{
		const D3D_FEATURE_LEVEL lvl[] = { D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0 };
		UINT createDeviceFlags = 0;
		D3D_FEATURE_LEVEL fl;

#ifdef _DEBUG_DX
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, lvl, _countof(lvl), D3D11_SDK_VERSION, &s_device, &fl, &s_context);
		if (FAILED(hr))
		{
			LOG(hr);
			return false;
		}

		ID3D11Multithread* multi;
		hr = s_device->QueryInterface(IID_PPV_ARGS(&multi));
		if (SUCCEEDED(hr) && multi != NULL)
		{
			multi->SetMultithreadProtected(TRUE);
			SAFE_RELEASE(multi);
		}
		else
			LOG_HR(hr);

		return true;
	}

	void ThomasCore::CreateDebugInterface()
	{
		typedef HRESULT(WINAPI * LPDXGIGETDEBUGINTERFACE)(REFIID, void **);

		HMODULE dxgidebug = LoadLibraryEx(_T("DXGIdebug.dll"), nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
		if (dxgidebug)
		{
			auto dxgiGetDebugInterface = reinterpret_cast<LPDXGIGETDEBUGINTERFACE>(
				reinterpret_cast<void*>(GetProcAddress(dxgidebug, "DXGIGetDebugInterface")));

			dxgiGetDebugInterface(IID_PPV_ARGS(&s_debug));
			HRESULT hr = dxgiGetDebugInterface(IID_PPV_ARGS(&s_infoQueue));

			if (SUCCEEDED(hr))
			{
				s_infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR, true);
				s_infoQueue->SetBreakOnSeverity(DXGI_DEBUG_ALL, DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION, true);

				for (int i = 0; i < s_infoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL); i++)
				{
					//Get the size of the message.
					SIZE_T messageLength;
					s_infoQueue->GetMessage(DXGI_DEBUG_ALL, 0, NULL, &messageLength);

					//Allocate space and get the message.
					DXGI_INFO_QUEUE_MESSAGE * pMessage = (DXGI_INFO_QUEUE_MESSAGE*)malloc(messageLength);
					s_infoQueue->GetMessage(DXGI_DEBUG_ALL, 0, pMessage, &messageLength);
					LOG(pMessage->pDescription);
				}

				s_infoQueue->ClearStoredMessages(DXGI_DEBUG_ALL);
			}
		}
	}
}

