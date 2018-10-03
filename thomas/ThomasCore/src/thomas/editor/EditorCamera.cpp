#include "EditorCamera.h"
#include "EditorGrid.h"
#include "..\Input.h"
#include "..\ThomasTime.h"
#include "..\WindowManager.h"
#include "..\resource\Shader.h"
#include "..\resource\Material.h"
#include "..\graphics\Renderer.h"
#include "..\graphics\render\Frame.h"
#include "..\resource\Model.h"
#include "..\Common.h"
#include "..\object\component\Camera.h"

namespace thomas
{
	namespace editor
	{
		EditorCamera EditorCamera::m_editorCamera;

		EditorCamera::EditorCamera() : 
			object::GameObject("editorCamera"), 
			m_sensitivity(1.f), 
			m_speed(2.f), 
			m_manipulatorScale(2.f), 
			m_hasSelectionChanged(false),
			m_manipulatorSnapping(false),
			m_selectedObject(nullptr),
			m_objectHighlighter(nullptr),
			m_manipulatorMode(ImGuizmo::MODE::LOCAL), 
			m_manipulatorOperation(ImGuizmo::OPERATION::TRANSLATE)
		{
			
		}

		EditorCamera::~EditorCamera()
		{
			
		}

		void EditorCamera::Destroy()
		{
			SAFE_DELETE(m_transform);
			m_cameraComponent.reset();
			m_grid.reset();
		}

		void EditorCamera::Init()
		{
			// Transform component
			m_transform = new object::component::Transform();
			m_transform->m_gameObject = this;
			m_transform->SetPosition(math::Vector3(5.f));
			m_transform->LookAt(math::Vector3());

			// Camera component
			m_cameraComponent = std::make_unique<object::component::Camera>(true);
			m_cameraComponent->SetTargetDisplay(-1);
			m_cameraComponent->m_gameObject = this;
			m_grid = std::unique_ptr<EditorGrid>(new EditorGrid(100, 1.f, 10));
			resource::Shader* outliner = resource::Shader::CreateShader("../Data/FXIncludes/EditorOutlineShader.fx");

			if (outliner)
			{
				m_objectHighlighter = std::make_unique<resource::Material>(outliner);
				m_objectHighlighter->m_renderQueue = 100;
			}
		}

		EditorCamera* EditorCamera::Instance()
		{
			return &m_editorCamera;
		}

		void EditorCamera::Render()
		{
			RenderCamera();
		}

		void EditorCamera::Update()
		{
			UpdateCamera();
		}

		void EditorCamera::SelectObject(object::GameObject * gameObject)
		{
			// Select single gameobject
			m_selectedObjects.clear();
			if (gameObject)
			{
				m_selectedObjects.push_back(gameObject);
			}

			m_hasSelectionChanged = true;
			m_selectedObject = gameObject;
		}

		void EditorCamera::UnselectObject(GameObject * gameObject)
		{
			// Unselect only a specific gameobject
			for (int i = 0; i < (int)m_selectedObjects.size(); ++i)
			{
				if (m_selectedObjects[i] == gameObject)
				{
					m_selectedObjects.erase(m_selectedObjects.begin() + i);
					--i;
				}
			}

			m_selectedObject = nullptr;
		}

		void EditorCamera::UnselectObjects()
		{
			// Unselect all objects in the scene
			m_selectedObjects.clear();
			m_hasSelectionChanged = true;
			m_selectedObject = nullptr;
		}

		const std::vector<object::GameObject*>& EditorCamera::GetSelectedObjects()
		{
			return m_selectedObjects;
		}

		void EditorCamera::SetHasSelectionChanged(const bool & selectionChanged)
		{
			m_hasSelectionChanged = selectionChanged;
		}

		bool EditorCamera::HasSelectionChanged()
		{
			return m_hasSelectionChanged;
		}

		object::component::Camera * EditorCamera::GetCamera() const
		{
			return m_cameraComponent.get();
		}

		void EditorCamera::SetManipulatorOperation(ImGuizmo::OPERATION operation)
		{
			m_manipulatorOperation = operation;
		}

		ImGuizmo::OPERATION EditorCamera::GetManipulatorOperation()
		{
			return m_manipulatorOperation;
		}

		void EditorCamera::ToggleManipulatorMode()
		{
			m_manipulatorMode = m_manipulatorMode == ImGuizmo::MODE::WORLD ? ImGuizmo::MODE::LOCAL : ImGuizmo::MODE::WORLD;
		}

		void EditorCamera::RenderCamera()
		{
			// Render camera related work
			RenderSelectedObjects();
			m_cameraComponent->Render();
			RenderGizmos();
			//Physics::DrawDebug(m_cameraComponent);
			m_grid->Draw(m_cameraComponent.get());

		}

		void EditorCamera::UpdateCamera()
		{
			// Make sure we are dealing with the editor window
			thomas::Window* window = WindowManager::Instance()->GetEditorWindow();
			if (!window)
				return;

			window->GetInput()->ResetScrollWheelValue();
			m_manipulatorSnapping = false;
			
			// Toggle editor mode on scene camera
			bool active = window->GetInput()->GetMouseButton(Input::MouseButtons::RIGHT);
			if (active)
				window->GetInput()->SetMouseMode(Input::MouseMode::POSITION_RELATIVE);
			else
				window->GetInput()->SetMouseMode(Input::MouseMode::POSITION_ABSOLUTE);

			if (window->GetInput()->GetKeyDown(Input::Keys::F))
				SnapCameraToFocus();


			if (active)
				MoveAndRotateCamera();
			else if (window->GetInput()->GetMouseButtonDown(Input::MouseButtons::LEFT))
			{
				if (!ImGuizmo::IsOver())
				{
					object::GameObject* gObj = FindClickedGameObject();
					SelectObject(gObj);
				}
			}
			else
			{
				// Allow manipulation of the gizmo if the game object is focused
				if (window->GetInput()->GetKeyDown(Input::Keys::W))
					m_manipulatorOperation = ImGuizmo::OPERATION::TRANSLATE;
				if (window->GetInput()->GetKeyDown(Input::Keys::R))
					m_manipulatorOperation = ImGuizmo::OPERATION::ROTATE;
				if (window->GetInput()->GetKeyDown(Input::Keys::E))
					m_manipulatorOperation = ImGuizmo::OPERATION::SCALE;
				if (window->GetInput()->GetKey(Input::Keys::LeftShift))
					m_manipulatorSnapping = true;
			}
		}

		void EditorCamera::RenderSelectedObjects()
		{
			// Render the selected objects for the editor camera
			if (!m_objectHighlighter)
				return;

			for (object::GameObject* gameObject : m_selectedObjects)
			{
				if (!gameObject->GetActive())
					continue;

				auto renderComponent = gameObject->GetComponent<object::component::RenderComponent>();
				if (renderComponent)
				{
					auto model = gameObject->GetComponent<object::component::RenderComponent>()->GetModel();
					if (model)
					{
						for (auto mesh : model->GetMeshes()) {
							graphics::render::RenderCommand cmd(
								gameObject->m_transform->GetWorldMatrix(),
								mesh.get(),
								m_objectHighlighter.get(),
								m_cameraComponent.get());
							graphics::Renderer::Instance()->SubmitCommand(cmd);
						}
					}
				}
			}
		}

		void EditorCamera::RenderGizmos()
		{
			float snap[] = { 1.f, 1.f, 1.f };

			for (unsigned i = 0; i < m_selectedObjects.size(); ++i)
			{
				object::GameObject* gameObject = m_selectedObjects[i];
				ImGuiIO& io = ImGui::GetIO();
				ImGuizmo::SetRect(0.f, 0.f, io.DisplaySize.x, io.DisplaySize.y);
				math::Matrix worldMatrix = gameObject->m_transform->GetWorldMatrix();

				if (m_manipulatorOperation == ImGuizmo::OPERATION::ROTATE)
					snap[0] = 15.f;

				math::Matrix deltaMatrix;
				ImGuizmo::Manipulate(
					*(m_cameraComponent->GetViewMatrix() * math::Matrix::CreateScale(m_manipulatorScale)).m, *m_cameraComponent->GetProjMatrix().m,
					m_manipulatorOperation, m_manipulatorMode, *worldMatrix.m, *deltaMatrix.m, m_manipulatorSnapping ? snap : 0);

				if (worldMatrix != gameObject->m_transform->GetWorldMatrix())
				{
					gameObject->m_transform->SetWorldMatrix(worldMatrix);
					gameObject->m_transform->SetDirty(true);
				}
			}
		}

		object::GameObject* EditorCamera::FindClickedGameObject()
		{
			math::Ray ray = m_cameraComponent->ScreenPointToRay(WindowManager::Instance()->GetEditorWindow()->GetInput()->GetMousePosition());
			auto renderComponents = object::Object::FindObjectsOfType<object::component::RenderComponent>();

			object::GameObject* closestGameObject = nullptr;
			float closestDistance = m_cameraComponent->GetFar();
			for (auto renderComponent : renderComponents)
			{
				float distance;
				if (renderComponent->m_bounds.Intersects(ray.position, ray.direction, distance) && distance < closestDistance)
				{
					closestDistance = distance;
					closestGameObject = renderComponent->m_gameObject;
				}
			}

			return closestGameObject;
		}

		void EditorCamera::MoveAndRotateCamera()
		{
			thomas::Window* window = WindowManager::Instance()->GetEditorWindow();
			if (!window)
				return;

			float speed = m_speed;
			// Increase camera speed
			if (window->GetInput()->GetKey(Input::Keys::LeftShift))
				speed *= 4.0f;

			// Allow the camera to move freely in the scene
			math::Vector3 right = m_transform->Right();
			math::Vector3 up = m_transform->Up();
			math::Vector3 forward = up.Cross(right);

			math::Vector3 translation;
			// Key input
			if (window->GetInput()->GetKey(Input::Keys::A))
				translation -= right * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Input::Keys::D))
				translation += right * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Input::Keys::W))
				translation += forward * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Input::Keys::S))
				translation -= forward * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Input::Keys::Q))
				translation -= up * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Input::Keys::E))
				translation += up * ThomasTime::GetActualDeltaTime() * speed;
			// Scroll
			m_transform->Translate(forward * ThomasTime::GetActualDeltaTime() * float(window->GetInput()->GetMouseScrollWheel()) * speed);
			// Apply translation
			m_transform->Translate(translation);
			
			float xStep = window->GetInput()->GetMouseX() * ThomasTime::GetActualDeltaTime() * m_sensitivity;
			float yStep = window->GetInput()->GetMouseY() * ThomasTime::GetActualDeltaTime() * m_sensitivity;
			// Rotate camera
			math::Quaternion rot = math::Quaternion::CreateFromAxisAngle(right, -yStep);
			rot *= math::Quaternion::CreateFromAxisAngle(math::Vector3::UnitY, -xStep);

			// Recreate matrix to prevent it from accumulating error
			up = math::Vector3::Transform(up, rot);
			forward = math::Vector3::Transform(forward, rot);
			m_transform->Orient(forward, up);
		}

		void EditorCamera::SnapCameraToFocus()
		{
			object::GameObject *gObj = m_selectedObject;
			if (gObj)
			{		
				math::Vector3 gPos = gObj->m_transform->GetPosition();
				math::Vector3 dir = m_transform->GetPosition() - gPos;
				dir.Normalize();

				m_transform->LookAt(gPos + math::Vector3(2.f) * dir, gPos, math::Vector3::Up);
			}
		}
	}
}

