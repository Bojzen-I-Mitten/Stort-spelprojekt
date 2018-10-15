#include "ThomasGUI.h"

// Thomas
#include "../../utils/d3d.h"
#include "../../WindowManager.h"
#include "../../utils/Utility.h"

namespace thomas
{
	namespace GUI
	{
		std::map<std::string, ThomasGUI::Image> ThomasGUI::m_images;
		std::map<std::string, ThomasGUI::Text> ThomasGUI::m_texts;
		std::unique_ptr<SpriteBatch> ThomasGUI::m_spriteBatch;
		std::map<std::wstring, std::unique_ptr<SpriteFont>> ThomasGUI::m_fonts;

		void ThomasGUI::Init()
		{
			m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
			m_fonts = std::map<std::wstring, std::unique_ptr<SpriteFont>>();
			s_defaultFont = std::make_unique<Font>(L"../Data/Fonts/CourierNew.spritefont");
		}

		void ThomasGUI::Destroy()
		{
			m_spriteBatch.reset();
		}

		void ThomasGUI::Render()
		{
			// Begin
			m_spriteBatch->Begin();

			for (const auto& image : m_images)
			{
				m_spriteBatch->Draw(image.second.texture->GetResourceView(), image.second.position, nullptr, image.second.color,
					image.second.rotation, Vector2(image.second.texture->GetWidth() / 2.f, image.second.texture->GetHeight() / 2.f),
					image.second.scale);
			}
			for (const auto& t : m_texts)
			{
				t.second.font->DrawGUIText(m_spriteBatch.get(), t.second.text, t.second.position, t.second.scale, t.second.color, t.second.rotation);
			}

			//End
			m_spriteBatch->End();
		}

		void ThomasGUI::Update()
		{
			m_images.clear();
			m_texts.clear();

		}

		void ThomasGUI::UpdateInteraction()
		{
			thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
			if (!window || WindowManager::Instance()->GetCurrentBound() != WindowManager::Instance()->GetEditorWindow())
				return;

			// Check intersection with interactable images
			for (auto& image : m_images)
			{
				if (image.second.interact)
				{
					// Construct boundaries
					Vector2 mousePos = window->GetInput()->GetMousePosition();
					Rect rect = { image.second.position.x,
								  image.second.position.x + image.second.texture->GetWidth() * image.second.scale.x,
								  image.second.position.y,
								  image.second.position.y + image.second.texture->GetHeight() * image.second.scale.y };

					if ((mousePos.x >= rect.left && mousePos.x <= rect.right) && (mousePos.y <= rect.down && mousePos.y >= rect.top))
					{
						image.second.intersected = true;
					}
					else
					{
						image.second.intersected = false;
					}
				}
			}
		}

		void ThomasGUI::AddImage(const std::string& id, Texture2D* texture, const Vector2& position, bool interact, 
								 const Vector4& color, const Vector2& scale, float rotation)
		{
			if (texture->GetResourceView())
			{
				Image image = { texture, position, scale, color, rotation, interact, false };
				m_images.insert(std::make_pair(id, image));
			}
		}

		void ThomasGUI::SetImageTexture(const std::string& id, Texture2D * texture)
		{
			auto image = GetImage(id);
			image.texture = texture;
		}

		void ThomasGUI::SetImagePosition(const std::string& id, const Vector2& position)
		{
			auto image = GetImage(id);
			image.position = position;
		}

		void ThomasGUI::SetImageColor(const std::string& id, const Vector4& color)
		{
			auto image = GetImage(id);
			image.color = color;
		}

		void ThomasGUI::SetImageScale(const std::string& id, const Vector2& scale)
		{
			auto image = GetImage(id);
			image.scale = scale;
		}

		void ThomasGUI::SetImageRotation(const std::string& id, float rotation)
		{
			auto image = GetImage(id);
			image.rotation = rotation;
		}

		void ThomasGUI::SetImageInteract(const std::string& id, bool interact)
		{
			auto image = GetImage(id);
			image.interact = interact;
		}

		bool ThomasGUI::OnImageClicked(const std::string& id)
		{
			thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
			if (!window || WindowManager::Instance()->GetCurrentBound() != WindowManager::Instance()->GetEditorWindow())
				return false;

			if (window->GetInput()->GetMouseButtonDown(Input::MouseButtons::LEFT))
			{
				return GetImage(id).intersected;
			}

			return false;
		}

		bool ThomasGUI::OnImageHovered(const std::string& id)
		{
			return GetImage(id).intersected;
		}

		ThomasGUI::Image & ThomasGUI::GetImage(const std::string& id)
		{
			auto found = m_images.find(id);

#ifdef _DEBUG
			assert(found != m_images.end());
#endif

			return found->second;
		}

		ThomasGUI::Text & ThomasGUI::GetText(const std::string & id)
		{
			return m_texts.find(id)->second;
		}

		void ThomasGUI::AddText(const std::string & id, std::string text, Font* font, const Vector2 & position, 
								const Vector2 & scale, float rotation, const Vector4 & color)
		{
			auto convertedText = thomas::utility::ToWChar(text).c_str();
			Text newText = { font, convertedText, position, scale, color, rotation };
			m_texts.insert(std::make_pair(id, newText));
		}

		void ThomasGUI::SetTextPosition(const std::string & id, const Vector2 & position)
		{
			ThomasGUI::Text text = GetText(id);
			text.position = position;
		}

		void ThomasGUI::SetTextColor(const std::string & id, const Vector4 & color)
		{
			ThomasGUI::Text text = GetText(id);
			text.color = color;
		}

		void ThomasGUI::SetTextScale(const std::string & id, const Vector2 & scale)
		{
			ThomasGUI::Text text = GetText(id);
			text.scale = scale;
		}

		void ThomasGUI::SetTextRotation(const std::string & id, float rotation)
		{
			ThomasGUI::Text text = GetText(id);
			text.rotation = rotation;
		}
	}
}