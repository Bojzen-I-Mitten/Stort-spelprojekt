#include "Camera.h"
#include "../../WindowManager.h"
#include "../GameObject.h"
#include "Transform.h"
#include "../../graphics/Renderer.h"
#include "../../Input.h"
#include "../../editor/gizmos/Gizmos.h"
#include "../../graphics/GUI/Canvas.h"
#include "../../graphics/GUI/GUIElements.h"
#include "RenderComponent.h"
#include "../../resource/texture/Texture2D.h"
#include "../../graphics/Skybox.h"
#include "../../Common.h"
#include "../../ThomasCore.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			void Camera::UpdateProjMatrix()
			{
				m_projMatrix = math::Matrix::CreatePerspectiveFieldOfView(math::DegreesToRadians(m_fov), GetViewport().AspectRatio(), m_near, m_far);
				m_frustrum = math::CreateFrustrumFromMatrixRH(m_projMatrix);
			}

			Camera::Camera() :
				Camera(0)
			{
			}
			Camera::Camera(int target_display) :
				m_ID(0),
				m_renderGUI(false)
				//m_GUIHandle(std::make_unique<graphics::GUIManager>())
			{
				m_fov = 70.f;
				m_near = 0.5;
				m_far = 10000.f;
				m_skybox = std::make_unique<graphics::SkyBox>();
				m_viewport = math::Viewport(0, 0, 1, 1);
				m_targetDisplay = target_display;
				UpdateProjMatrix();
			}


			Camera::~Camera()
			{
				m_canvases.clear();
				graphics::Renderer::Instance()->getCameraList().remove(this);
			}

			void Camera::OnEnable()
			{
				m_ID = graphics::Renderer::Instance()->getCameraList().add(this);
			}

			void Camera::OnDisable()
			{
				//m_skybox.reset();
				graphics::Renderer::Instance()->getCameraList().remove(this);
				m_ID = 0;
			}

			void Camera::OnDestroy()
			{
				m_renderGUI = false;
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
				{
					return math::Viewport(m_viewport.x, m_viewport.y, m_viewport.width * window->GetWidth(), m_viewport.height * window->GetHeight());
				}
				else
				{
					return m_viewport;
				}
			}

			math::Vector2 Camera::GetViewportScale()
			{
				return Vector2(m_viewport.width, m_viewport.height);
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

			void Camera::Render()
			{
				//PROFILE(__FUNCSIG__, thomas::ProfileManager::operationType::miscLogic)
				//graphics::Renderer::Instance()->SubmitCamera(this);
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
				editor::Gizmos::Gizmo().SetMatrix(m_gameObject->m_transform->GetWorldMatrix());
				editor::Gizmos::Gizmo().SetColor(math::Color(0.6f, 0.6f, 0.6f));
				editor::Gizmos::Gizmo().DrawFrustum(m_frustrum);
			
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

				math::Rectangle window = math::Rectangle((long)GetViewport().x, (long)GetViewport().y, (long)GetViewport().width, (long)GetViewport().height);
				math::Vector2 center = window.Center();
				window.Offset((long)-center.x, (long)-center.y);
				rect.Offset((long)-center.x, (long)-center.y);


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
				m_frameData.targetDisplay = GetTargetDisplayIndex();
				m_frameData.viewport = GetViewport();
				m_frameData.viewMatrix = GetViewMatrix();
				m_frameData.projectionMatrix = GetProjMatrix();

				m_frameData.position = (math::Vector4)GetPosition();

				//if (m_frameData.targetDisplay == 0)
				//{
				//	for (int i = 0; i < m_canvases.size(); ++i)
				//	{
				//		m_canvases[i]->SetViewport(m_frameData.viewport);
				//	}
				//}
				graphics::Renderer::Instance()->SubmitCamera(this);
			}

			const graphics::render::CAMERA_FRAME_DATA& Camera::GetFrameData()
			{
				//CopyFrameData();
				return m_frameData;
			}

			uint32_t Camera::ID()
			{
				return m_ID;
			}

			graphics::GUI::Canvas* Camera::AddCanvas()
			{
				std::unique_ptr<graphics::GUI::Canvas> canvas = std::make_unique<graphics::GUI::Canvas>(m_viewport, this);
				m_canvases.push_back(std::move(canvas));

				return m_canvases[m_canvases.size() - 1].get();
			}

			graphics::GUI::Canvas * Camera::AddCanvas(math::Viewport viewport)
			{
				std::unique_ptr<graphics::GUI::Canvas> canvas = std::make_unique<graphics::GUI::Canvas>(viewport, this);
				m_canvases.push_back(std::move(canvas));

				return m_canvases[m_canvases.size() - 1].get();
			}

			void Camera::RenderGUI()
			{
				for (int i = 0; i < m_canvases.size(); ++i)
				{
					m_canvases[i]->Render();
				}
			}


			void Camera::SetSkyMap(resource::TextureCube * tex)
			{
				m_skybox->SetSkyMap(tex);
			}

			resource::TextureCube * Camera::GetSkyMap()
			{
				return m_skybox->GetSkyMap();
			}
			
			void Camera::DrawSkyBox()
			{
				m_skybox->Draw();
			}
			bool Camera::hasSkybox()
			{
				return bool(m_skybox);
			}

			math::Vector3 Camera::WorldToViewport(math::Vector3 position, math::Matrix world)
			{
				math::Vector3 NDC = math::Vector3::Transform(position, world * GetViewMatrix() * GetProjMatrix());

				NDC.x = (NDC.x + 1.0f) * 0.5f * GetViewport().width;
				NDC.y = (1.0f - NDC.y) * 0.5f * GetViewport().height;

				return NDC;
			}
		}
	}
}