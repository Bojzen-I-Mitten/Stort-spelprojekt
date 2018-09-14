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

namespace thomas
{
	namespace graphics
	{
		CommandQueue Renderer::s_renderCommands;
		CommandQueue Renderer::s_lastFramesCommands;

		void Renderer::BindFrame()
		{
			//Per frame
			float realDeltaTime = ThomasTime::GetActualDeltaTime();
			float dt = ThomasTime::GetDeltaTime();
			math::Vector4 thomas_DeltaTime(realDeltaTime, 1.f / realDeltaTime, dt, 1.f / dt);
			resource::Shader::SetGlobalVector(THOMAS_DELTA_TIME, thomas_DeltaTime);

			LightManager::Bind();
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
			s_renderCommands.clear();
		}

		void Renderer::SubmitCommand(RenderCommand command)
		{
			s_renderCommands[command.camera][command.material].push_back(command);
		}

		void Renderer::TransferCommandList()
		{
			s_lastFramesCommands = s_renderCommands;
		}

		void Renderer::BindObject(RenderCommand &rC)
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
			for (auto & perCameraQueue : s_lastFramesCommands)
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
		bool MaterialSorter::operator()(resource::Material * mat1, resource::Material * mat2) const
		{
			if (mat1->m_renderQueue == mat2->m_renderQueue)
				return mat1->GetId() < mat2->GetId();
			else
				return mat1->m_renderQueue < mat2->m_renderQueue;
		}

	}
}

