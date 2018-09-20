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
#include "..\Window.h"
#include "RenderConstants.h"
#include "render/Frame.h"

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
			resource::Shader::SetGlobalVector(THOMAS_DELTA_TIME, thomas_DeltaTime);

			LightManager::Bind();
		}

		Renderer::Renderer()
			: m_frame(new render::Frame(100)), m_prevFrame(new render::Frame(100))
		{
		}

		void Renderer::BindCamera(thomas::object::component::Camera * camera)
		{
			//Get the current active window
			int displayIndex = camera->GetTargetDisplayIndex();
			auto window = Window::GetWindow(displayIndex);

			if (!window || !window->Initialized())
				return;

			window->Bind();
			ThomasCore::GetDeviceContext()->RSSetViewports(1, camera->GetViewport().Get11());

			//Set global camera properties
			resource::Shader::SetGlobalMatrix(THOMAS_MATRIX_PROJECTION, camera->GetProjMatrix().Transpose());
			resource::Shader::SetGlobalMatrix(THOMAS_MATRIX_VIEW, camera->GetViewMatrix().Transpose());
			resource::Shader::SetGlobalMatrix(THOMAS_MATRIX_VIEW_INV, camera->GetViewMatrix().Invert());
			resource::Shader::SetGlobalMatrix(THOMAS_MATRIX_VIEW_PROJ, camera->GetViewProjMatrix().Transpose());
			resource::Shader::SetGlobalVector(THOMAS_VECTOR_CAMERA_POS, (math::Vector4)camera->GetPosition());
		}

		void Renderer::ClearCommands()
		{
			m_frame->m_queue.clear();
		}

		void Renderer::SubmitCommand(render::RenderCommand& command)
		{
			if (command.num_local_prop) {
				resource::shaderproperty::ShaderProperty** alloc;
				if (m_frame->m_alloc.reserve(command.num_local_prop, alloc))
					return;
				for (unsigned int i = 0; i < command.num_local_prop; i++)
					alloc[i] = command.local_prop[i]->copy();
				command.local_prop = const_cast<const resource::shaderproperty::ShaderProperty**>(alloc);
			}
			m_frame->m_queue[command.camera][command.material].push_back(command);
		}

		void Renderer::TransferCommandList()
		{
			// Swap frames and clear old frame data
			std::swap(m_frame, m_prevFrame);
			m_frame->clear();
		}

		void Renderer::BindObject(render::RenderCommand &rC)
		{
			thomas::resource::shaderproperty::ShaderProperty* prop;
			rC.material->SetMatrix(THOMAS_MATRIX_WORLD, rC.worldMatrix.Transpose());
			rC.material->ApplyProperty(THOMAS_MATRIX_WORLD);

			rC.material->SetMatrix(THOMAS_MATRIX_WORLD_INV, rC.worldMatrix.Invert());
			rC.material->ApplyProperty(THOMAS_MATRIX_WORLD_INV);

			for (unsigned int i = 0; i < rC.num_local_prop; i++)
				rC.local_prop[i]->Apply(rC.material->GetShader());
		}

		void Renderer::ProcessCommands()
		{
			//Process commands
			BindFrame();
			for (auto & perCameraQueue : m_prevFrame->m_queue)
			{
				auto camera = perCameraQueue.first;
				BindCamera(camera);
				for (auto & perMaterialQueue : perCameraQueue.second)
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

			//Take care of the editor camera and render gizmos
			if (editor::EditorCamera::GetEditorCamera())
			{
				BindCamera(editor::EditorCamera::GetEditorCamera()->GetCamera());
				editor::Gizmos::RenderGizmos();
			}
		}

	}
}

