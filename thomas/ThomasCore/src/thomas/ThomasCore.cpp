#include "ThomasCore.h"
#include "Sound.h"
#include "Input.h"
#include "Window.h"
#include "ThomasTime.h"
#include "object\Object.h"
#include "resource\texture\Texture2D.h"
#include "resource\Shader.h"
#include "resource\Material.h"
#include "editor\EditorCamera.h"
#include "editor\gizmos\Gizmos.h"
#include "utils\Primitives.h"
#include "utils\d3d.h"
#include <d3d11_4.h>
#include <comdef.h>

#include "object/component/LightComponent.h"

namespace thomas 
{
	std::vector<std::string> ThomasCore::s_logOutput;
	bool ThomasCore::s_clearLog;
	bool ThomasCore::s_initialized;
	ImGuiContext* ThomasCore::s_imGuiContext;

	bool ThomasCore::Init()
	{
		s_imGuiContext = ImGui::CreateContext();
		s_logOutput.reserve(10);

		//Init all required classes
		InitDirectX();
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
		utils::D3D::Destroy();

		return true;
	}

	ID3D11Device * ThomasCore::GetDevice()
	{
		return utils::D3D::GetDevice();
	}

	ID3D11DeviceContext* ThomasCore::GetDeviceContext()
	{
		return utils::D3D::GetDeviceContext();
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
		utils::D3D::Init();
		return true;
	}
}

