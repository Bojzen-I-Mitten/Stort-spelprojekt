#include "Renderer.h"
#include "..\utils\D3D.h"
#include "..\object\GameObject.h"
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
#include "../graphics/GUI/Canvas.h"
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
			m_shaders(),
			m_cameras()
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
		}

		void Renderer::Destroy()
		{
			m_shaders.Destroy();
		}

		Renderer* Renderer::Instance()
		{
			static Renderer s_renderer;
			return &s_renderer;
		}

		void Renderer::BindCamera(const render::CAMERA_FRAME_DATA& frameData)
		{
			//Get the current active window

			if (!BindCameraViewport(frameData))
				return;

			math::Matrix viewProjMatrix = frameData.viewMatrix * frameData.projectionMatrix;

			//Set global camera properties
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_PROJECTION, frameData.projectionMatrix.Transpose());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW, frameData.viewMatrix.Transpose());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW_INV, frameData.viewMatrix.Invert());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW_PROJ, viewProjMatrix.Transpose());
			m_shaders.SetGlobalVector(THOMAS_VECTOR_CAMERA_POS, frameData.position);
		}
		bool Renderer::BindCameraViewport(const render::CAMERA_FRAME_DATA& frameData)
		{
			//Get the current active window

			auto window = WindowManager::Instance()->GetWindow(frameData.targetDisplay);

			if (!window || !window->Initialized())
				return false;

			window->Bind();
			utils::D3D::Instance()->GetDeviceContext()->RSSetViewports(1, frameData.viewport.Get11());
			return true;
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
		void Renderer::SubmitCamera(object::component::Camera* cam)
		{
			m_frame->m_queue[cam->ID()].m_frameData = cam->GetFrameData();
		}
		void Renderer::SubmitCommand(render::RenderCommand& command)
		{
			m_frame->m_queue[command.camera_ID].m_commands3D[command.material].push_back(command);
		}

		render::Frame & Renderer::getAllocator()
		{
			return *m_frame;
		}

		void Renderer::TransferCommandList()
		{

			// Sync. update
			m_shaders.SyncList();
			m_cameras.syncUpdate();
			// Submit camera frame data...
			for (object::component::Camera* camera : m_cameras.getCameras())
				camera->CopyFrameData();
			editor::EditorCamera::Instance()->GetCamera()->CopyFrameData();

			// Swap frames and clear old frame data
			std::swap(m_frame, m_prevFrame);
			m_frame->clear();
		}

		const render::ShaderList & Renderer::getShaderList()
		{
			return m_shaders;
		}

		const render::CameraList & Renderer::getCameraList()
		{
			return m_cameras;
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
			utils::profiling::GpuProfiler* profiler = utils::D3D::Instance()->GetProfiler();
			
			//Process commands
			BindFrame();

			for (auto & perCameraQueue : m_prevFrame->m_queue)
			{
				BindCamera(perCameraQueue.second.m_frameData);

				// Skyboxes should be submitted!
				object::component::Camera* camera = m_cameras.getCamera(perCameraQueue.first);
				if (camera && camera->hasSkybox())
					camera->DrawSkyBox();
				// Draw objects
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
			}
	
			profiler->Timestamp(utils::profiling::GTS_MAIN_OBJECTS);

			ParticleSystem::GetGlobalAlphaBlendingSystem()->UpdateParticleSystem();
			ParticleSystem::GetGlobalAdditiveBlendingSystem()->UpdateParticleSystem();
			if (editor::EditorCamera::Instance())
			{
				BindCamera(editor::EditorCamera::Instance()->GetCamera()->GetFrameData());
				ParticleSystem::GetGlobalAlphaBlendingSystem()->DrawParticles();
				ParticleSystem::GetGlobalAdditiveBlendingSystem()->DrawParticles();
			}
			for (object::component::Camera* cam : m_cameras.getCameras())
			{
				BindCamera(cam->GetFrameData());
				ParticleSystem::GetGlobalAlphaBlendingSystem()->DrawParticles();
				ParticleSystem::GetGlobalAdditiveBlendingSystem()->DrawParticles();
			}
			profiler->Timestamp(utils::profiling::GTS_PARTICLES);
			

			//Take care of the editor camera and render gizmos
			if (editor::EditorCamera::Instance())
			{
				BindCamera(editor::EditorCamera::Instance()->GetCamera()->GetFrameData());
				editor::Gizmos::Gizmo().RenderGizmos();
			}

			// Gui draw
			for (auto & perCameraQueue : m_prevFrame->m_queue)
			{
				// Draw GUI for each camera that has enabled GUI rendering
				// Shitty solution to camera destruction:
				object::component::Camera* camera = m_cameras.getCamera(perCameraQueue.first);

				BindCameraViewport(perCameraQueue.second.m_frameData);
				if (camera && camera->GetGUIRendering())
					camera->RenderGUI();
			}

			profiler->Timestamp(utils::profiling::GTS_GIZMO_OBJECTS);
		}

	}
}


