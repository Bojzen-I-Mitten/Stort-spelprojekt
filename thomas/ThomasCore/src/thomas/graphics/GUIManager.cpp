#include "GUIManager.h"

// Thomas
#include "../utils/d3d.h"
#include "../WindowManager.h"

namespace thomas
{
	namespace graphics
	{
		GUIManager::GUIManager() :
		m_spriteBatch(nullptr)
		{
			m_spriteBatch = new SpriteBatch(utils::D3D::Instance()->GetDeviceContext());
			m_defaultFont = new Font("../Data/Fonts/CourierNew.spritefont");
		}

		void GUIManager::Destroy()
		{
			m_images.clear();

			delete m_defaultFont;
			m_defaultFont = nullptr;

			delete m_spriteBatch;
			m_spriteBatch = nullptr;
		}

		void GUIManager::Render()
		{
			// Begin
			if (m_spriteBatch != nullptr)
			{
				m_spriteBatch->Begin();

				for (const auto& image : m_images)
				{
					m_spriteBatch->Draw(image.second.texture->GetResourceView(), image.second.position, nullptr, image.second.color,
										image.second.rotation, Vector2(0.f, 0.f), image.second.scale);
				}

				for (const auto& text : m_texts)
				{
					text.second.font->DrawGUIText(m_spriteBatch, text.second.text, text.second.position, text.second.scale, 
												  text.second.color, text.second.rotation);
				}

				// End
				m_spriteBatch->End();
			}
		}

		// Images
		void GUIManager::AddImage(const std::string& id, Texture2D* texture, const Vector2& position, bool interact,
								  const Vector4& color, const Vector2& scale, float rotation)
		{
			if (texture->GetResourceView())
			{
				Image image = { texture, position, scale, color, rotation, interact };
				m_images.insert(std::make_pair(id, image));
			}
		}

		void GUIManager::SetImageTexture(const std::string& id, Texture2D * texture)
		{
			auto& image = GetImage(id);
			image.texture = texture;
		}

		void GUIManager::SetImagePosition(const std::string& id, const Vector2& position)
		{
			auto& image = GetImage(id);
			image.position = position;
		}

		void GUIManager::SetImageColor(const std::string& id, const Vector4& color)
		{
			auto& image = GetImage(id);
			image.color = color;
		}

		void GUIManager::SetImageScale(const std::string& id, const Vector2& scale)
		{
			auto& image = GetImage(id);
			image.scale = scale;
		}

		void GUIManager::SetImageRotation(const std::string& id, float rotation)
		{
			auto& image = GetImage(id);
			image.rotation = rotation;
		}

		void GUIManager::SetImageInteract(const std::string& id, bool interact)
		{
			auto& image = GetImage(id);
			image.interact = interact;
		}

		bool GUIManager::OnImageClicked(const std::string& id)
		{
			thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
			if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
				return false;

			if (window->GetInput()->GetMouseButtonDown(Input::MouseButtons::LEFT))
			{
				return CheckImageIntersection(id);
			}

			return false;
		}

		bool GUIManager::OnImageHovered(const std::string& id)
		{
			return CheckImageIntersection(id);
		}

		GUIManager::Image& GUIManager::GetImage(const std::string& id)
		{
			auto found = m_images.find(id);

#ifdef _DEBUG
			assert(found != m_images.end());
#endif

			return found->second;
		}

		bool GUIManager::CheckImageIntersection(const std::string& id)
		{
			thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
			if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
				return false;

			if (!m_images.empty())
			{
				auto image = GetImage(id);

				if (image.interact)
				{
					// Construct boundaries
					// Note: if origin has to be changed from (0, 0) this also has to be taken into account when constructing the boundaries!
					Vector2 mousePos = window->GetInput()->GetMousePosition();
					Rect rect = { image.position.x,
								  image.position.x + image.texture->GetWidth() * image.scale.x,
								  image.position.y,
								  image.position.y + image.texture->GetHeight() * image.scale.y };

					if ((mousePos.x >= rect.left && mousePos.x <= rect.right) && (mousePos.y <= rect.down && mousePos.y >= rect.top))
					{
						return true;
					}
					else
					{
						return false;
					}
				}
			}

			return false;
		}

		// Text
		GUIManager::Text & GUIManager::GetText(const std::string& id)
		{
			auto found = m_texts.find(id);

#ifdef _DEBUG
			assert(found != m_texts.end());
#endif

			return found->second;
		}

		void GUIManager::AddText(const std::string& id, std::string text, const Vector2& position,
								 const Vector2& scale, float rotation, const Vector4& color, Font* font)
		{
			if (font == nullptr)
				font = m_defaultFont;

			Text newText = { font, text, position, scale, color, rotation };
			m_texts.insert(std::make_pair(id, newText));
		}

		void GUIManager::SetTextPosition(const std::string& id, const Vector2& position)
		{
			auto& text = GetText(id);
			text.position = position;
		}

		void GUIManager::SetTextColor(const std::string& id, const Vector4& color)
		{
			auto& text = GetText(id);
			text.color = color;
		}

		void GUIManager::SetTextScale(const std::string& id, const Vector2& scale)
		{
			auto& text = GetText(id);
			text.scale = scale;
		}

		void GUIManager::SetTextRotation(const std::string& id, float rotation)
		{
			auto& text = GetText(id);
			text.rotation = rotation;
		}

		void GUIManager::SetText(const std::string& id, std::string newText)
		{
			auto& text = GetText(id);
			text.text = newText;
		}
	}
}