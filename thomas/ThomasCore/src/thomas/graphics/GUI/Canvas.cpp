// Thomas
#include "Canvas.h"
#include "GUIElements.h"
#include "../../utils/d3d.h"
#include "../../WindowManager.h"
#include "../../Common.h"
#include "../../ThomasCore.h"
#include "../../object/component/Camera.h"

namespace thomas
{
	namespace graphics
	{
		namespace GUI
		{
			Canvas::Canvas(Viewport viewport, object::component::Camera* cam, Vector2 baseResolution)
			{
				m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
				m_defaultFont = std::make_unique<Font>("../Data/Fonts/CourierNew.spritefont");
				m_spriteStates = std::make_unique<CommonStates>(utils::D3D::Instance()->GetDevice());
				m_camera = cam;
				m_viewport = viewport;
				m_baseResolution = baseResolution;
				m_render = true;
				m_GUIElements = std::vector<std::unique_ptr<GUIElement>>();
				Set3D(false);
			}

			void Canvas::Destroy()
			{
				m_spriteBatch.reset();
				m_defaultFont.reset();
				m_spriteStates.reset();
				m_GUIElements.clear();
			}

			void Canvas::Render()
			{
				if (m_spriteBatch != nullptr && m_render)
				{		
					math::Matrix matrix = math::Matrix::Identity;
					if (Get3D())
					{
						matrix = m_worldMatrix * m_camera->GetViewProjMatrix();
					}

					m_spriteBatch->SetViewport(GetViewport());
					m_spriteBatch->Begin(SpriteSortMode_BackToFront, m_spriteStates->NonPremultiplied(), nullptr, nullptr, m_spriteStates->CullNone(), nullptr, matrix);

					Vector2 vpScale(GetViewport().width / 1920.0f, GetViewport().height / 1080.0f);
					for (int i = 0; i < m_GUIElements.size(); ++i)
					{
						m_GUIElements[i]->Draw(m_spriteBatch.get(), GetViewport(), vpScale);
					}
					
					m_spriteBatch->End();
				}
			}

			Viewport Canvas::GetViewport()
			{
				if (Get3D())
					return Viewport(0, 0, 1920, 1080);

				Viewport camViewport = m_camera->GetViewport();
				return Viewport(
					camViewport.x + m_viewport.x * camViewport.width, 
					camViewport.y + m_viewport.y * camViewport.height,
					m_viewport.width * camViewport.width, 
					m_viewport.height * camViewport.height);
			}

			Vector2 Canvas::GetViewportScale()
			{
				Viewport vp = GetViewport();
				return Vector2(vp.width / 1920.f , vp.height / 1080.f);
			}

			void Canvas::SetViewport(Viewport viewport)
			{
				m_viewport = viewport;
			}

			GUIElement* Canvas::Add(const std::string& text)
			{
				Font* font = m_defaultFont.get();
				std::unique_ptr<GUIElement> newText =
					std::unique_ptr<GUIElement>(new Text(font, text, this));
				m_GUIElements.push_back(std::move(newText));

				return m_GUIElements[m_GUIElements.size() - 1].get();
			}

			GUIElement* Canvas::Add(Texture2D * texture)
			{
				if (texture->GetResourceView())
				{
					std::unique_ptr<GUIElement> image = std::make_unique<Image>(texture, this);
					m_GUIElements.push_back(std::move(image));

					return m_GUIElements[m_GUIElements.size() - 1].get();
				}

				return nullptr;
			}

			void Canvas::Remove(GUIElement* _element)
			{
				auto element = m_GUIElements.begin();

				while (element._Ptr->get() != _element && element != m_GUIElements.end())
					element++;

				if (element != m_GUIElements.end())
				{
					m_GUIElements.erase(element);
				}
			}

			void Canvas::SetRendering(bool render)
			{
				m_render = render;
			}
			bool Canvas::GetRendering()
			{
				return m_render;
			}
			void Canvas::SetWorldMatrix(math::Matrix value)
			{
				m_worldMatrix = value;
			}
			void Canvas::Set3D(bool value)
			{
				if(value)
					m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_UNSPECIFIED);
				else
					m_spriteBatch->SetRotation(DXGI_MODE_ROTATION_IDENTITY);
			}
			bool Canvas::Get3D()
			{
				return m_spriteBatch->GetRotation() == DXGI_MODE_ROTATION_UNSPECIFIED;
			}
		}
	}
}