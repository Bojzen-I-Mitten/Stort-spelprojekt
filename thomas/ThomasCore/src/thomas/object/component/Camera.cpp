#include "Camera.h"
#include "../../WindowManager.h"
#include "../GameObject.h"
#include "../../graphics/Skybox.h"
#include "Transform.h"
#include "../../graphics/Renderer.h"
#include "../../Input.h"
#include "../../editor/gizmos/Gizmos.h"
#include "../../AutoProfile.h"
#include "../../graphics/GUIManager.h"
#include "RenderComponent.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			std::vector<Camera*> Camera::s_allCameras;

			void Camera::UpdateProjMatrix()
			{
				m_projMatrix = math::Matrix::CreatePerspectiveFieldOfView(math::DegreesToRadians(m_fov), GetViewport().AspectRatio(), m_near, m_far);
				m_frustrum = math::CreateFrustrumFromMatrixRH(m_projMatrix);
			}

			Camera::Camera(bool dontAddTolist) : 
			m_renderGUI(false),
			m_GUIHandle(std::make_unique<graphics::GUIManager>())
			{
				m_fov = 70.f;
				m_near = 0.1f;
				m_far = 10000.f;
				m_viewport = math::Viewport(0, 0, 1, 1);
				m_targetDisplay = -1;
				UpdateProjMatrix();
			}

			Camera::Camera() :
			m_renderGUI(false),
			m_GUIHandle(std::make_unique<graphics::GUIManager>())
			{
				m_fov = 70;
				m_near = 0.5;
				m_far = 10000;
				m_viewport = math::Viewport(0, 0, 1,1);
				m_targetDisplay = 0;
				UpdateProjMatrix();
				
				std::sort(s_allCameras.begin(), s_allCameras.end(), [](Camera* a, Camera* b) 
				{
					return a->GetTargetDisplayIndex() < b->GetTargetDisplayIndex();
				});
			}

			Camera::~Camera()
			{
				m_GUIHandle->Destroy();
				m_GUIHandle.reset();
				

				for (int i = 0; i < s_allCameras.size(); i++)
				{
					if (s_allCameras[i] == this)
					{
						s_allCameras.erase(s_allCameras.begin() + i);
						break;
					}		
				}
			}

			void Camera::OnEnable()
			{
				s_allCameras.push_back(this);
			}

			void Camera::OnDisable()
			{
				m_renderGUI = false;
				for (int i = 0; i < s_allCameras.size(); i++) 
				{
					if (s_allCameras[i] == this) 
					{
						

						s_allCameras.erase(s_allCameras.begin() + i);
						return;
					}
						
				}
			}

			math::Matrix Camera::GetProjMatrix()
			{
				UpdateProjMatrix();
				return m_projMatrix;
			}

			math::Matrix Camera::GetViewMatrix()
			{
				math::Matrix viewMatrix =  m_gameObject->m_transform->GetWorldMatrix();
				return viewMatrix.Invert();
			}

			math::Matrix Camera::GetViewProjMatrix()
			{
				return GetViewMatrix() * m_projMatrix;
			}

			math::Vector3 Camera::GetPosition()
			{
				return m_gameObject->m_transform->GetPosition();
			}

			math::Vector3 Camera::GetDirection()
			{
				return m_gameObject->m_transform->Forward();
			}

			math::Ray Camera::ScreenPointToRay(math::Vector2 point)
			{
				PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
				// Move the mouse cursor coordinates into the -1 to +1 range.
				Window* window = WindowManager::Instance()->GetWindow(m_targetDisplay);

				float pointX = ((2.0f * (float)point.x) / (float)window->GetWidth()) - 1.0f;
				float pointY = (((2.0f * (float)point.y) / (float)window->GetHeight()) - 1.0f) * -1.0f;
				// Adjust the points using the projection matrix to account for the aspect ratio of the viewport.
				pointX /= m_projMatrix._11;
				pointY /= m_projMatrix.Transpose()._22;


				// Get the inverse of the view matrix.
				math::Matrix inverseViewMatrix = GetViewMatrix().Invert();

				math::Vector3 direction(pointX, pointY, -1.0f);

				// Calculate the direction of the picking ray in view space.
				direction = math::Vector3::TransformNormal(direction, inverseViewMatrix);

				// Get the origin of the picking ray which is the position of the camera
				math::Vector3 origin = GetPosition();
				direction.Normalize();
				return math::Ray(origin, direction);
			}

			float Camera::GetFov()
			{
				return m_fov;
			}

			float Camera::GetNear()
			{
				return m_near;
			}

			float Camera::GetFar()
			{
				return m_far;
			}

			void Camera::SetFov(float fov)
			{
				m_fov = fov;
				UpdateProjMatrix();
			}

			void Camera::SetNear(float viewNear)
			{
				if (viewNear > 0)
					m_near = viewNear;
				UpdateProjMatrix();
			}

			void Camera::SetFar(float viewFar)
			{
				if (viewFar > 0)
					m_far = viewFar;
				UpdateProjMatrix();
			}

			bool Camera::GetGUIRendering() const
			{
				return m_renderGUI;
			}

			void Camera::SetGUIRendering(bool rendering)
			{
				m_renderGUI = rendering;
			}

			math::Viewport Camera::GetViewport()
			{
				Window* window = WindowManager::Instance()->GetWindow(m_targetDisplay);

				if (window)
					return math::Viewport(m_viewport.x, m_viewport.y, m_viewport.width * window->GetWidth(), m_viewport.height * window->GetHeight());
				else
					return m_viewport;
			}

			void Camera::SetViewport(math::Viewport viewport)
			{
				m_viewport = viewport;
				UpdateProjMatrix();
			}

			void Camera::SetViewport(float x, float y, float width, float height)
			{
				SetViewport(math::Viewport(x, y, width, height));
			}

			float Camera::GetAspectRatio()
			{
				return m_viewport.AspectRatio();
			}

			graphics::GUIManager * Camera::GetGUIHandle() const
			{
				return m_GUIHandle.get();
			}

			void Camera::Render()
			{
				PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
				for (RenderComponent* renderComponent : RenderComponent::GetAllRenderComponents())
				{
					if(renderComponent->m_gameObject->GetActive())
						renderComponent->SubmitToRenderer(this);
				}
			}

			void Camera::OnDrawGizmos()
			{	
			}

			void Camera::OnDrawGizmosSelected()
			{
				//editor::Gizmos::SetMatrix(m_gameObject->m_transform->GetWorldMatrix().Transpose());
				editor::Gizmos::Gizmo().SetColor(math::Color(0.6f, 0.6f, 0.6f));
				editor::Gizmos::Gizmo().DrawFrustum(GetFrustrum());
			
			}

			void Camera::SetTargetDisplay(int index)
			{
				if (WindowManager::Instance()->GetNumOfWindows() < index)
				{
					m_targetDisplay = index;
					UpdateProjMatrix();
				}	
			}

			int Camera::GetTargetDisplayIndex()
			{
				return m_targetDisplay;
			}

			math::BoundingFrustum Camera::GetFrustrum()
			{
				math::BoundingFrustum frustrum;
				m_frustrum.Transform(frustrum, m_gameObject->m_transform->GetWorldMatrix());
				return frustrum;
			}

			math::BoundingFrustum Camera::GetSubFrustrum(math::Rectangle rect)
			{
				
				math::BoundingFrustum subFrustrum(GetFrustrum());

				math::Rectangle window = math::Rectangle(GetViewport().x, GetViewport().y, GetViewport().width, GetViewport().height);
				math::Vector2 center = window.Center();
				window.Offset(-center.x, -center.y);
				rect.Offset(-center.x, -center.y);


				if (rect.width < 0)
				{
					rect.x = rect.x + rect.width;
					rect.width = abs(rect.width);
				}

				if (rect.height < 0)
				{
					rect.y = rect.y + rect.height;
					rect.height = abs(rect.height);
				}

				float left = (float)rect.x / (float)window.x;
				float right = (float)(rect.x + rect.width) / (float)(window.x + window.width);
				float top = (float)rect.y / (float)window.y;
				float bottom = (float)(rect.y + rect.height) / (float)(window.y + window.height);

				subFrustrum.LeftSlope *= left;
				subFrustrum.RightSlope *= right;
				subFrustrum.TopSlope *= top;
				subFrustrum.BottomSlope *= bottom;
				
				return subFrustrum;
			}

			void Camera::CopyFrameData()
			{
				PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
				m_frameData.targetDisplay = GetTargetDisplayIndex();
				m_frameData.viewport = GetViewport();
				m_frameData.viewMatrix = GetViewMatrix();
				m_frameData.projectionMatrix = GetProjMatrix();

				m_frameData.position = (math::Vector4)GetPosition();

			}

			Camera::CAMERA_FRAME_DATA & Camera::GetFrameData()
			{
				return m_frameData;
			}
		}
	}
}