#include "Renderer.h"
#include "..\utils\D3D.h"
#include "..\object\GameObject.h"
#include "..\object\component\Camera.h"
#include "..\object\component\Transform.h"
#include "..\ThomasCore.h"
#include "..\ThomasTime.h"
#include "..\resource\Material.h"
#include "..\resource\Shader.h"
#include "..\resource\ShaderProperty\ShaderProperty.h"
#include "..\editor\gizmos\Gizmos.h"
#include "..\editor\EditorCamera.h"
#include "..\WindowManager.h"
#include "..\graphics\LightManager.h"
#include "RenderConstants.h"
#include "render/Frame.h"
#include "../utils/GpuProfiler.h"
#include "../graphics/GUIManager.h"
#include "ParticleSystem.h"

namespace thomas
{
	namespace graphics
	{
		void Renderer::BindFrame()
		{
			//Per frame
			float realDeltaTime = ThomasTime::GetActualDeltaTime();
			float dt = ThomasTime::GetDeltaTime();
			math::Vector4 thomas_DeltaTime(realDeltaTime, 1.f / realDeltaTime, dt, 1.f / dt);
			m_shaders.SetGlobalVector(THOMAS_DELTA_TIME, thomas_DeltaTime);

			LightManager::Bind(&m_shaders);
		}

		constexpr uint32_t NUM_STRUCT = 200;
		constexpr uint32_t NUM_MATRIX = 5000;
		Renderer::Renderer()	: 
			m_frame(new render::Frame(NUM_STRUCT, 64 * NUM_MATRIX)), 
			m_prevFrame(new render::Frame(NUM_STRUCT, 64 * NUM_MATRIX)),
			m_shaders()
		{
			
		}

		Renderer::~Renderer()
		{
			//m_shaders~ShaderList();	Implicit call
		}

		void Renderer::init()
		{
			// Do something?
		}

		void Renderer::PostRender()
		{
			m_shaders.SyncList();
		}

		void Renderer::Destroy()
		{
			this->~Renderer();
		}

		Renderer* Renderer::Instance()
		{
			static Renderer s_renderer;
			return &s_renderer;
		}

		void Renderer::BindCamera(thomas::object::component::Camera * camera)
		{
			object::component::Camera::CAMERA_FRAME_DATA& frameData = camera->GetFrameData();
			//Get the current active window

			auto window = WindowManager::Instance()->GetWindow(frameData.targetDisplay);

			if (!window || !window->Initialized())
				return;

			window->Bind();
			utils::D3D::Instance()->GetDeviceContext()->RSSetViewports(1, frameData.viewport.Get11());

			math::Matrix viewProjMatrix = frameData.viewMatrix * frameData.projectionMatrix;

			//Set global camera properties
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_PROJECTION, frameData.projectionMatrix.Transpose());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW, frameData.viewMatrix.Transpose());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW_INV, frameData.viewMatrix.Invert());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW_PROJ, viewProjMatrix.Transpose());
			m_shaders.SetGlobalVector(THOMAS_VECTOR_CAMERA_POS, frameData.position);
		}

		void Renderer::ClearCommands()
		{
			m_frame->m_queue.clear();
		}

		void Renderer::ClearAllCommands()
		{
			m_frame->m_queue.clear();
			m_prevFrame->m_queue.clear();
		}

		void Renderer::SubmitCommand(render::RenderCommand& command)
		{
			m_frame->m_queue[command.camera].m_commands3D[command.material].push_back(command);
		}

		render::Frame & Renderer::getAllocator()
		{
			return *m_frame;
		}

		void Renderer::TransferCommandList()
		{
			// Swap frames and clear old frame data
			std::swap(m_frame, m_prevFrame);
			m_frame->clear();
		}

		const render::ShaderList & Renderer::getShaderList()
		{
			return m_shaders;
		}

		resource::Shader * Renderer::GetStandardShader()
		{
			return m_shaders.GetStandardShader();
		}


		void Renderer::BindObject(render::RenderCommand &rC)
		{
			thomas::resource::shaderproperty::ShaderProperty* prop;
			rC.material->SetMatrix(THOMAS_MATRIX_WORLD, rC.worldMatrix.Transpose());
			rC.material->ApplyProperty(THOMAS_MATRIX_WORLD);

			rC.material->SetMatrix(THOMAS_MATRIX_WORLD_INV, rC.worldMatrix.Invert());
			rC.material->ApplyProperty(THOMAS_MATRIX_WORLD_INV);

			for (unsigned int i = 0; i < rC.num_local_prop; i++)
				rC.local_prop[i].m_apply(rC.local_prop[i], rC.material->GetShader());
		}

		void Renderer::ProcessCommands()
		{
			profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
			
			//Process commands
			BindFrame();

			//ParticleSystem::GetGlobalSystem()->UpdateParticleSystem();
			//m_particleSystem->UpdateParticleSystem();

			for (auto & perCameraQueue : m_prevFrame->m_queue)
			{
				auto camera = perCameraQueue.first;
				BindCamera(camera);
				for (auto & perMaterialQueue : perCameraQueue.second.m_commands3D)
				{
					auto material = perMaterialQueue.first;
					material->Bind();
					for (auto & perMeshCommand : perMaterialQueue.second)
					{
						BindObject(perMeshCommand);
						material->Draw(perMeshCommand.mesh);
					}
				}

				//m_particleSystem->DrawParticles();	

				// Draw GUI for each camera that has enabled GUI rendering
				if (perCameraQueue.first->GetGUIRendering())
				{
					perCameraQueue.first->GetGUIHandle()->Render();
				}
			}
	
			profiler->Timestamp(profiling::GTS_MAIN_OBJECTS);

			ParticleSystem::GetGlobalSystem()->UpdateParticleSystem();
			if (editor::EditorCamera::Instance())
			{
				BindCamera(editor::EditorCamera::Instance()->GetCamera());
				ParticleSystem::GetGlobalSystem()->DrawParticles();
			}
			for (object::component::Camera* cam : object::component::Camera::s_allCameras)
			{
				BindCamera(cam);
				ParticleSystem::GetGlobalSystem()->DrawParticles();
			}
			profiler->Timestamp(profiling::GTS_PARTICLES);
			

			//Take care of the editor camera and render gizmos
			if (editor::EditorCamera::Instance())
			{
				BindCamera(editor::EditorCamera::Instance()->GetCamera());
				editor::Gizmos::Gizmo().RenderGizmos();
			}

			profiler->Timestamp(profiling::GTS_GIZMO_OBJECTS);
		}

	}
}


