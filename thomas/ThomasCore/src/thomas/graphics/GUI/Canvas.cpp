// Thomas
#include "Canvas.h"
#include "GUIElements.h"
#include "../../utils/d3d.h"
#include "../../WindowManager.h"
#include "../../Common.h"
#include "../../ThomasCore.h"


namespace thomas
{
	namespace graphics
	{
		namespace GUI
		{
			Canvas::Canvas(Viewport viewport, Viewport* camViewport, Vector2 baseResolution)
			{
				m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContextImmediate());
				m_defaultFont = std::make_unique<Font>("../Data/Fonts/CourierNew.spritefont");
				m_spriteStates = std::make_unique<CommonStates>(utils::D3D::Instance()->GetDevice());
				m_viewport = viewport;
				m_camViewport = camViewport;
				m_viewportScale = Vector2(1.f, 1.f);
				m_baseResolution = baseResolution;
				m_render = true;

				m_GUIElements = std::vector<std::unique_ptr<GUIElement>>();
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
					m_spriteBatch->Begin(SpriteSortMode_BackToFront, m_spriteStates->NonPremultiplied());

					for (int i = 0; i < m_GUIElements.size(); ++i)
					{
						m_GUIElements[i]->Draw(m_spriteBatch.get(), m_viewport, m_viewportScale);
					}

					m_spriteBatch->End();
				}
			}

			Viewport Canvas::GetViewport()
			{
				return m_viewport;
			}

			Vector2 Canvas::GetViewportScale()
			{
				return m_viewportScale;
			}

			void Canvas::SetViewport(Viewport viewport)
			{
				m_viewport.x		= m_camViewport->x + viewport.x * m_camViewport->width;
				m_viewport.width	= m_camViewport->x + viewport.width * m_camViewport->width;
				m_viewport.y		= m_camViewport->y + viewport.y * m_camViewport->height;
				m_viewport.height	= m_camViewport->x + viewport.height * m_camViewport->height;

				UpdateViewportScale();
			}

			void Canvas::UpdateViewportScale()
			{
				m_viewportScale = Vector2(m_viewport.width - m_viewport.x, m_viewport.height - m_viewport.y) *
					Vector2(m_camViewport->width - m_camViewport->x, m_camViewport->height - m_camViewport->y) / m_baseResolution;
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
		}
	}
}