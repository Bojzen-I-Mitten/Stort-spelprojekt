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
#include "utils/ThreadMap.h"

#include "object/ObjectHandler.h"
#include "object/component/LightComponent.h"
#include "Physics.h"
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
		Sound::Init();
		graphics::Renderer::Instance()->init();

		resource::Material::Init();
		Physics::Init();
		editor::EditorCamera::Instance()->Init();
		editor::Gizmos::Gizmo().Init();

		graphics::LightManager::Initialize();
		graphics::ParticleSystem::InitializeGlobalSystems();

		ObjectHandler::Init();
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

		Sound::Update();
	}

	void ThomasCore::Render()
	{
		profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
		profiler->BeginFrame();
		WindowManager::Instance()->BeginFrame();
		profiler->Timestamp(profiling::GTS_MAIN_CLEAR);
		graphics::Renderer::Instance()->ProcessCommands();

		// Draw performance readout - at end of CPU frame, so hopefully the previous frame
		//  (whose data we're getting) will have finished on the GPU by now.

		profiler->WaitForDataAndUpdate();
		WindowManager::Instance()->EndFrame();
		utils::D3D::Instance()->GetProfiler()->EndFrame();

		graphics::Renderer::Instance()->PostRender();	// Sync. shaders ...?
	}

	void ThomasCore::Exit()
	{
		s_initialized = false;
	}

	ThomasCore::ThomasCore()
		: m_threadMap(new utils::ThreadMap(MAX_NUM_THREAD)), m_memAlloc(new resource::MemoryAllocation())
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
		graphics::ParticleSystem::DestroyGlobalSystems();
		graphics::Renderer::Instance()->Destroy();
		resource::Material::Destroy();
		resource::Texture2D::Destroy();
		editor::EditorCamera::Instance()->Destroy();
		editor::Gizmos::Gizmo().Destroy();
		utils::Primitives::Destroy();
		Physics::Destroy();
		Sound::Destroy();
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

	utils::ThreadMap & ThomasCore::getThreadMap()
	{
		return *m_threadMap;
	}
	void ThomasCore::registerThread()
	{
		m_threadMap->registerThread();
	}
	uint32_t ThomasCore::Thread_Index()
	{
		return m_threadMap->Thread_Index();
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

