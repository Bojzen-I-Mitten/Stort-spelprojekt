#include "ThomasCore.h"
#include "Sound.h"
#include "Input.h"
#include "WindowManager.h"
#include "ThomasTime.h"
#include "editor\Editor.h"
#include "object\Object.h"
#include "resource\texture\Texture2D.h"
#include "resource\Shader.h"
#include "resource\Material.h"
#include "resource/MemoryAllocation.h"
#include "editor\EditorCamera.h"
#include "editor\gizmos\Gizmos.h"
#include "utils\Primitives.h"
#include "utils\d3d.h"
#include <d3d11_4.h>
#include <comdef.h>
#include "AutoProfile.h"
#include "utils/GpuProfiler.h"
#include "graphics/Renderer.h"
#include "graphics\ParticleSystem.h"

namespace thomas 
{
	std::vector<std::string> ThomasCore::s_logOutput;
	bool ThomasCore::s_clearLog;
	bool ThomasCore::s_initialized;
	bool ThomasCore::s_isEditor = false;
	ImGuiContext* ThomasCore::s_imGuiContext;

	bool ThomasCore::Init()
	{
		s_initialized = false;
		s_imGuiContext = ImGui::CreateContext();
		s_logOutput.reserve(10);

		//Init all required classes
		if (!utils::D3D::Instance()->Init())
			return false;

		resource::Texture2D::Init();
		ThomasTime::Init();
		Sound::Instance()->Init();
		resource::Shader::Init();

		resource::Material::Init();
		Physics::Init();
		editor::EditorCamera::Instance()->Init();
		editor::Gizmos::Init();

		graphics::LightManager::Initialize();
		graphics::ParticleSystem::InitializeGlobalSystem();

		s_initialized = true;
		return s_initialized;
	}

	void ThomasCore::Update()
	{
		PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
		if (s_clearLog)
		{
			s_logOutput.clear();
			s_clearLog = false;
		}

		object::Object::Clean();
		resource::Shader::Update();	
		Sound::Instance()->Update();
	}

	void ThomasCore::Render()
	{
		profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
		profiler->BeginFrame();
		WindowManager::Instance()->ClearAllWindows();
		profiler->Timestamp(profiling::GTS_MAIN_CLEAR);
		graphics::Renderer::Instance()->ProcessCommands();

		// Draw performance readout - at end of CPU frame, so hopefully the previous frame
		//  (whose data we're getting) will have finished on the GPU by now.

		profiler->WaitForDataAndUpdate();
		WindowManager::Instance()->PresentAllWindows();
		utils::D3D::Instance()->GetProfiler()->EndFrame();
	}

	void ThomasCore::Exit()
	{
		s_initialized = false;
	}

	ThomasCore::ThomasCore()
		: m_memAlloc(new resource::MemoryAllocation())
	{
	}

	bool ThomasCore::Initialized()
	{
		return s_initialized;
	}

	bool ThomasCore::Destroy()
	{	
		//Destroy all objects
		WindowManager::Instance()->Destroy();
		graphics::LightManager::Destroy();
		graphics::ParticleSystem::DestroyGlobalSystem();
		resource::Shader::DestroyAllShaders();
		resource::Material::Destroy();
		resource::Texture2D::Destroy();
		object::Object::Destroy();
		editor::EditorCamera::Instance()->Destroy();
		editor::Gizmos::Destroy();
		utils::Primitives::Destroy();
		Physics::Destroy();
		Sound::Instance()->Destroy();
		ImGui::DestroyContext(s_imGuiContext);
		utils::D3D::Instance()->Destroy();

		return true;
	}

	std::vector<std::string> ThomasCore::GetLogOutput()
	{
		return s_logOutput;
	}

	ThomasCore & ThomasCore::Core()
	{
		static ThomasCore core;
		return core;
	}

	void ThomasCore::OnStop()
	{
		graphics::Renderer::Instance()->ClearAllCommands();

#ifdef _EDITOR
		editor::Editor::GetEditor().OnEditorStop();
#endif
	}
	void ThomasCore::OnPlay()
	{
#ifdef _EDITOR
		thomas::editor::Editor::GetEditor().OnEditorPlay();
#endif
	}

	resource::MemoryAllocation * ThomasCore::Memory()
	{
		return  m_memAlloc;
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
	bool ThomasCore::IsEditor()
	{
		return s_isEditor;
	}
	void ThomasCore::SetEditor(bool value)
	{
		s_isEditor = value;
	}
}

