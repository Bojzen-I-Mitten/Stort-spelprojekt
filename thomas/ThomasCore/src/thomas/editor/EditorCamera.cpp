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
			m_hasSelectionChanged(false),
			m_manipulatorSnapping(false),
			m_isBoxSelecting(false),
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
				UnselectObject(gameObject);
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

		bool EditorCamera::IsObjectSelected(GameObject * gameObject)
		{
			for (int i = 0; i < (int)m_selectedObjects.size(); ++i)
			{
				if (m_selectedObjects[i] == gameObject)
				{
					return true;
				}
			}
			return false;
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

		void EditorCamera::ToggleObjectSelection(GameObject * gameObject)
		{
			if (!gameObject)
				return;
			//Deselect if already selected
			for (int i = 0; i < (int)m_selectedObjects.size(); ++i)
			{
				if (m_selectedObjects[i] == gameObject)
				{
					m_selectedObjects.erase(m_selectedObjects.begin() + i);
					return;
				}
			}
			
			m_selectedObjects.push_back(gameObject);
			m_hasSelectionChanged = true;
			m_selectedObject = gameObject;
		}

		void EditorCamera::RenderCamera()
		{
			// Render camera related work
			RenderSelectedObjects();
			m_cameraComponent->Render();
			RenderGizmos();
			Physics::DrawDebug(m_cameraComponent.get());
			m_grid->Draw(m_cameraComponent.get());

		}

		void EditorCamera::UpdateCamera()
		{
			// Make sure we are dealing with the editor window
			thomas::Window* window = WindowManager::Instance()->GetEditorWindow();
			if (!window || WindowManager::Instance()->GetCurrentBound() != window)
				return;

			window->GetInput()->ResetScrollWheelValue();
			m_manipulatorSnapping = false;
			
			// Toggle editor mode on scene camera
			bool active = window->GetInput()->GetMouseButton(Input::MouseButtons::RIGHT);
			if (active)
				window->GetInput()->SetMouseMode(Input::MouseMode::POSITION_RELATIVE);
			else
				window->GetInput()->SetMouseMode(Input::MouseMode::POSITION_ABSOLUTE);

			if (window->GetInput()->GetKeyDown(Keys::F))
				SnapCameraToFocus();


			if (active)
				MoveAndRotateCamera();
			else if (window->GetInput()->GetMouseButtonUp(Input::MouseButtons::LEFT))
			{
				if (m_isBoxSelecting)
				{
					m_isBoxSelecting = false;
					return;
				}
				if (!ImGuizmo::IsOver())
				{
					object::GameObject* gObj = FindClickedGameObject();
					if (window->GetInput()->GetKey(Keys::LeftControl))
						ToggleObjectSelection(gObj);
					else
						SelectObject(gObj);
				}
			}
			else if (window->GetInput()->GetMouseButtonDown(Input::MouseButtons::LEFT))
			{
				if (!ImGuizmo::IsUsing())
				{
					BeginBoxSelect();
				}
			}
			else if (window->GetInput()->GetMouseButton(Input::MouseButtons::LEFT))
			{
				if (!ImGuizmo::IsOver() || m_isBoxSelecting)
				{
					BoxSelect();
				}
			}
			else
			{
				// Allow manipulation of the gizmo if the game object is focused
				if (window->GetInput()->GetKeyDown(Keys::W))
					m_manipulatorOperation = ImGuizmo::OPERATION::TRANSLATE;
				if (window->GetInput()->GetKeyDown(Keys::R))
					m_manipulatorOperation = ImGuizmo::OPERATION::ROTATE;
				if (window->GetInput()->GetKeyDown(Keys::E))
					m_manipulatorOperation = ImGuizmo::OPERATION::SCALE;
				if (window->GetInput()->GetKey(Keys::LeftShift))
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
			if (m_selectedObjects.empty())
				return;
			float snap[] = { 1.f, 1.f, 1.f };

			math::Vector3 averagePosition = math::Vector3::Zero;
			math::Vector3 averageScale = math::Vector3::Zero;
			math::Quaternion rot = m_selectedObjects[0]->m_transform->GetRotation();
			std::vector<math::Matrix> offsetMatrixes = std::vector<math::Matrix>(m_selectedObjects.size());
			for (unsigned i = 0; i < m_selectedObjects.size(); ++i)
			{
				object::GameObject* gameObject = m_selectedObjects[i];
				averagePosition += gameObject->m_transform->GetPosition();
				averageScale += gameObject->m_transform->GetScale();
			}
			averagePosition /= m_selectedObjects.size();
			averageScale /= m_selectedObjects.size();
			


			math::Matrix parentMatrix = math::Matrix::CreateScale(averageScale) * math::Matrix::CreateFromQuaternion(rot) *
				math::Matrix::CreateTranslation(averagePosition);

			for (unsigned i = 0; i < m_selectedObjects.size(); ++i)
			{
				object::GameObject* gameObject = m_selectedObjects[i];
				offsetMatrixes[i] = gameObject->m_transform->GetWorldMatrix() * parentMatrix.Invert();
			}

			ImGuiIO& io = ImGui::GetIO();
			ImGuizmo::SetRect(0.f, 0.f, io.DisplaySize.x, io.DisplaySize.y);

			if (m_manipulatorOperation == ImGuizmo::OPERATION::ROTATE)
				snap[0] = 15.f;

			float scale = 1080.0f / io.DisplaySize.x;
			math::Matrix deltaMatrix;
			ImGuizmo::Manipulate(
				*(m_cameraComponent->GetViewMatrix() * math::Matrix::CreateScale(scale)).m, *m_cameraComponent->GetProjMatrix().m,
				m_manipulatorOperation, m_manipulatorMode, *parentMatrix.m, *deltaMatrix.m, m_manipulatorSnapping ? snap : 0);

			if (ImGuizmo::IsUsing())
			{
				for (unsigned i = 0; i < m_selectedObjects.size(); ++i)
				{
					object::GameObject* gameObject = m_selectedObjects[i];

					gameObject->m_transform->SetWorldMatrix(offsetMatrixes[i] * parentMatrix);
					gameObject->m_transform->SetDirty(true);
				}
			}

		}

		object::GameObject* EditorCamera::FindClickedGameObject()
		{
			math::Ray ray = m_cameraComponent->ScreenPointToRay(WindowManager::Instance()->GetEditorWindow()->GetInput()->GetMousePosition());
			auto renderComponents = object::component::RenderComponent::GetAllRenderComponents();

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

		void EditorCamera::BoxSelect()
		{
			math::Vector2 mousePos = WindowManager::Instance()->GetEditorWindow()->GetInput()->GetMousePosition();

			m_boxSelectRect.width = mousePos.x - m_boxSelectRect.x;
			m_boxSelectRect.height = mousePos.y - m_boxSelectRect.y;

			if (abs(m_boxSelectRect.width) < 10 && abs(m_boxSelectRect.height) < 10)
				return;

			m_isBoxSelecting = true;

			ImGui::GetOverlayDrawList()->AddRectFilled(
				ImVec2(m_boxSelectRect.x, m_boxSelectRect.y),
				ImVec2(m_boxSelectRect.x + m_boxSelectRect.width, m_boxSelectRect.y + m_boxSelectRect.height),
				ImColor(1.0f, 1.0f, 1.0f, 0.3f));

			ImGui::GetOverlayDrawList()->AddRect(
				ImVec2(m_boxSelectRect.x, m_boxSelectRect.y),
				ImVec2(m_boxSelectRect.x + m_boxSelectRect.width, m_boxSelectRect.y + m_boxSelectRect.height),
				ImColor(1.0f, 1.0f, 1.0f, 1.0f));

			math::BoundingFrustum frustrum = m_cameraComponent->GetSubFrustrum(m_boxSelectRect);
			
			
			auto renderComponents = object::component::RenderComponent::GetAllRenderComponents();
			for (auto renderComponent : renderComponents)
			{
				bool IsAlreadySelected = IsObjectSelected(renderComponent->m_gameObject);
				if (frustrum.Contains(renderComponent->m_bounds))
				{
					if (!IsAlreadySelected) {
						m_selectedObjects.push_back(renderComponent->m_gameObject);
						m_selectedObject = renderComponent->m_gameObject;
						m_hasSelectionChanged = true;
					}

				}
				else if (IsAlreadySelected) {
					UnselectObject(renderComponent->m_gameObject);
					m_hasSelectionChanged = true;
				}

			}
			
			


		}

		void EditorCamera::BeginBoxSelect()
		{
			math::Vector2 mousePos = WindowManager::Instance()->GetEditorWindow()->GetInput()->GetMousePosition();
			m_boxSelectRect = math::Rectangle(mousePos.x, mousePos.y, 0, 0);
		}

		void EditorCamera::MoveAndRotateCamera()
		{
			thomas::Window* window = WindowManager::Instance()->GetEditorWindow();
			if (!window)
				return;

			float speed = m_speed;
			// Increase camera speed
			if (window->GetInput()->GetKey(Keys::LeftShift))
				speed *= 4.0f;

			// Allow the camera to move freely in the scene
			math::Vector3 right = m_transform->Right();
			math::Vector3 up = m_transform->Up();
			math::Vector3 forward = up.Cross(right);

			math::Vector3 translation;
			// Key input
			if (window->GetInput()->GetKey(A))
				translation -= right * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Keys::D))
				translation += right * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Keys::W))
				translation += forward * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Keys::S))
				translation -= forward * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Keys::Q))
				translation -= up * ThomasTime::GetActualDeltaTime() * speed;
			if (window->GetInput()->GetKey(Keys::E))
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
			if (m_selectedObjects.empty())
				return;

			math::BoundingBox combinedBox = math::BoundingBox(m_selectedObjects[0]->m_transform->GetPosition(), math::Vector3::Zero);
			for each(GameObject* gameObject in m_selectedObjects)
			{
				auto renderComponent = gameObject->GetComponent<object::component::RenderComponent>();
				math::BoundingBox boundingBox;
				if (renderComponent)
				{
					boundingBox = math::BoundingBox(renderComponent->m_bounds.Center, renderComponent->m_bounds.Extents);
				}
				else
				{
					boundingBox = math::BoundingBox(gameObject->m_transform->GetPosition(), math::Vector3::Zero);
				}

				math::BoundingBox::CreateMerged(combinedBox, boundingBox, combinedBox);
			}

			m_transform->LookAt(combinedBox.Center);
			m_transform->SetPosition(combinedBox.Center);
			m_transform->Translate(-m_transform->Forward()*combinedBox.Extents*5.0f);
		}
	}
}

