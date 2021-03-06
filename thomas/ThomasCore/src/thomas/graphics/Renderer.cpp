#include "Renderer.h"
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
#include "render\Frame.h"
#include "..\utils\GpuProfiler.h"
#include "..\utils\AutoProfile.h"
#include "..\graphics\GUI\Canvas.h"
#include "ParticleSystem.h"
#include "..\resource\texture\RenderTexture.h"
namespace thomas
{
	namespace graphics
	{
		void Renderer::BindFrame()
		{
			//Per frame
			float elapsed = ThomasTime::GetElapsedTime();
			float realDeltaTime = ThomasTime::GetActualDeltaTime();
			float dt = ThomasTime::GetDeltaTime();
			math::Vector4 thomas_Time(elapsed / 20.0f, elapsed, elapsed*2.0f, elapsed*3.0f);
			math::Vector4 thomas_DeltaTime(realDeltaTime, 1.f / realDeltaTime, dt, 1.f / dt);
			m_shaders.SetGlobalVector(THOMAS_TIME, thomas_Time);
			m_shaders.SetGlobalVector(THOMAS_DELTA_TIME, thomas_DeltaTime);

			LightManager::BindLights(&m_shaders);
		}

		constexpr uint32_t NUM_STRUCT = 200;
		constexpr uint32_t NUM_MATRIX = 5000;
		Renderer::Renderer() :
			m_frame(new render::Frame(NUM_STRUCT, 64 * NUM_MATRIX)),
			m_prevFrame(new render::Frame(NUM_STRUCT, 64 * NUM_MATRIX)),
			m_shaders(),
			m_cameras()
		{
			m_commmandList[0] = nullptr;
			m_commmandList[1] = nullptr;
			m_commmandList[2] = nullptr;
			m_commmandList[3] = nullptr;
			m_commmandList[4] = nullptr;
			m_enableShadows = true;
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
			SAFE_RELEASE(m_commmandList[0]);
			SAFE_RELEASE(m_commmandList[1]);
			SAFE_RELEASE(m_commmandList[2]);
			SAFE_RELEASE(m_commmandList[3]);
			SAFE_RELEASE(m_commmandList[4]);
		}

		Renderer* Renderer::Instance()
		{
			static Renderer s_renderer;
			return &s_renderer;
		}

		bool Renderer::BindCameraRenderTarget(const render::CAMERA_FRAME_DATA& frameData)
		{
			if (frameData.renderTexture) 
			{
				frameData.renderTexture->Bind();
			}
			else
			{
				//Get the current active window
				auto window = WindowManager::Instance()->GetWindow(frameData.targetDisplay);

				if (!window || !window->Initialized())
					return false;

				window->BindRenderTarget();
			}

			utils::D3D::Instance()->GetDeviceContextDeferred()->RSSetViewports(1, frameData.viewport.Get11());

			math::Matrix viewProjMatrix = frameData.viewMatrix * frameData.projectionMatrix;

			//Set global camera properties
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_PROJECTION, frameData.projectionMatrix.Transpose());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW, frameData.viewMatrix.Transpose());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW_INV, frameData.viewMatrix.Invert());
			m_shaders.SetGlobalMatrix(THOMAS_MATRIX_VIEW_PROJ, viewProjMatrix.Transpose());
			m_shaders.SetGlobalVector(THOMAS_VECTOR_CAMERA_POS, frameData.position);

			return true;
		}

		bool Renderer::BindCameraBackBuffer(const render::CAMERA_FRAME_DATA& frameData)
		{
			//Get the current active window
			auto window = WindowManager::Instance()->GetWindow(frameData.targetDisplay);

			if (!window || !window->Initialized())
				return false;

			window->BindBackBuffer();
			utils::D3D::Instance()->GetDeviceContextDeferred()->RSSetViewports(1, frameData.viewport.Get11());
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
			rC.material->SetMatrix(THOMAS_MATRIX_WORLD, rC.worldMatrix.Transpose());
			rC.material->ApplyProperty(THOMAS_MATRIX_WORLD);

			rC.material->SetMatrix(THOMAS_MATRIX_WORLD_INV, rC.worldMatrix.Invert());
			rC.material->ApplyProperty(THOMAS_MATRIX_WORLD_INV);

			for (unsigned int i = 0; i < rC.num_local_prop; i++)
				rC.local_prop[i].m_apply(rC.local_prop[i], rC.material->GetShader());
		}

		void Renderer::ProcessCommands()
		{
			//Process commands
			{
				PROFILE("BindFrame")
					BindFrame();
			}


			/*utils::D3D::Instance()->ResetCommandList(m_commmandList[0]);*/
			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_SHADOWS_BEGIN);

			if (m_enableShadows)
			{
				for (auto & perCameraQueue : m_prevFrame->m_queue)
				{
					object::component::Camera* camera = m_cameras.getCamera(perCameraQueue.first);

					LightManager::DrawShadows(perCameraQueue.second, camera);
				}
			}

			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_SHADOWS_END);
		/*	utils::D3D::Instance()->FinishCommandList(m_commmandList[0]);
			utils::D3D::Instance()->ExecuteCommandList(m_commmandList[0]);*/

			/*utils::D3D::Instance()->ResetCommandList(m_commmandList[1]);*/
			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_MAIN_OBJECTS_BEGIN);

			for (auto & perCameraQueue : m_prevFrame->m_queue)
			{
				object::component::Camera* camera = m_cameras.getCamera(perCameraQueue.first);

				{
					PROFILE("CameraBind")
					BindCameraRenderTarget(perCameraQueue.second.m_frameData);
				}
				// Skyboxes should be submitted!
				{
					PROFILE("CameraDrawSkybox")
					if (camera && camera->hasSkybox())
						camera->DrawSkyBox();
				}

				// Draw objects
				if (m_enableShadows)
				{
					LightManager::BindShadows(&m_shaders);
				}

				{
					PROFILE("CameraDrawObjects")
					for (auto & perMaterialQueue : perCameraQueue.second.m_commands3D)
					{
						auto material = perMaterialQueue.first;
						{
							PROFILE("BindMaterial")
							
							
							material->Bind();
						}
						{
							PROFILE("CameraDrawObjects")
							for (auto & perMeshCommand : perMaterialQueue.second)
							{
								{
									PROFILE("BindObject")
									BindObject(perMeshCommand);
								}
								{
									PROFILE("DrawCall")
									material->Draw(perMeshCommand.mesh);
								}
							}
						}
					}
				}
			}

			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_MAIN_OBJECTS_END);
			/*utils::D3D::Instance()->FinishCommandList(m_commmandList[1]);
			utils::D3D::Instance()->ExecuteCommandList(m_commmandList[1]);*/

			/*utils::D3D::Instance()->ResetCommandList(m_commmandList[2]);*/
			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_PARTICLES_BEGIN);

			{
				PROFILE("Particles");
				ParticleSystem::GetGlobalAlphaBlendingSystem()->UpdateParticleSystem();
				ParticleSystem::GetGlobalAdditiveBlendingSystem()->UpdateParticleSystem();
				if (editor::EditorCamera::Instance())
				{
					object::component::Camera* camera = editor::EditorCamera::Instance()->GetCamera();
					BindCameraRenderTarget(camera->GetFrameData());
					ParticleSystem::GetGlobalAlphaBlendingSystem()->DrawParticles();
					ParticleSystem::GetGlobalAdditiveBlendingSystem()->DrawParticles();
				}
				for (object::component::Camera* cam : m_cameras.getCameras())
				{
					BindCameraRenderTarget(cam->GetFrameData());
					ParticleSystem::GetGlobalAlphaBlendingSystem()->DrawParticles();
					ParticleSystem::GetGlobalAdditiveBlendingSystem()->DrawParticles();
				}
			}

			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_PARTICLES_END);
			/*utils::D3D::Instance()->FinishCommandList(m_commmandList[2]);
			utils::D3D::Instance()->ExecuteCommandList(m_commmandList[2]);*/

			/*utils::D3D::Instance()->ResetCommandList(m_commmandList[3]);*/
			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_GIZMO_OBJECTS_BEGIN);

			{
				PROFILE("Gizmos");
				//Take care of the editor camera and render gizmos
				if (editor::EditorCamera::Instance())
				{
					object::component::Camera* camera = editor::EditorCamera::Instance()->GetCamera();
					BindCameraRenderTarget(camera->GetFrameData());
					editor::Gizmos::Gizmo().RenderGizmos();
				}
			}

			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_GIZMO_OBJECTS_END);
			/*utils::D3D::Instance()->FinishCommandList(m_commmandList[3]);
			utils::D3D::Instance()->ExecuteCommandList(m_commmandList[3]);*/

			//Copy rendered objects into the back buffer
			WindowManager::Instance()->ResolveRenderTarget();

		/*	utils::D3D::Instance()->ResetCommandList(m_commmandList[4]);*/
			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_GUI_BEGIN);

			{
				PROFILE("GUI");
				// Gui draw
				for (auto & perCameraQueue : m_prevFrame->m_queue)
				{
					// Draw GUI for each camera that has enabled GUI rendering
					// Shitty solution to camera destruction:
					object::component::Camera* camera = m_cameras.getCamera(perCameraQueue.first);

					BindCameraBackBuffer(perCameraQueue.second.m_frameData);
					if (camera && camera->GetGUIRendering())
						camera->RenderGUI();
				}
			}

			utils::profiling::GpuProfiler::Instance()->Timestamp(utils::profiling::GTS_GUI_END);
			/*utils::D3D::Instance()->FinishCommandList(m_commmandList[4]);
			utils::D3D::Instance()->ExecuteCommandList(m_commmandList[4]);*/
		}
	}
}


