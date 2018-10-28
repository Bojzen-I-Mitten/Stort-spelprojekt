// Thomas
#include "GUIManager.h"
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
			GUIManager::GUIManager()
			{
				m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
				m_defaultFont = std::make_unique<Font>("../Data/Fonts/CourierNew.spritefont");
				m_spriteStates = std::make_unique<CommonStates>(utils::D3D::Instance()->GetDevice());
				m_viewport = Vector2(1920.f, 1080.f);
				m_viewportScale = Vector2(1.f, 1.f);
			}

			void GUIManager::Destroy()
			{
				m_spriteBatch.reset();
				m_defaultFont.reset();
				m_spriteStates.reset();
				/*m_texts.clear();
				m_images.clear();*/
				for (auto element : m_GUIElements)
				{
					delete element;
				}
				m_GUIElements.clear();
			}

			//void GUIManager::Render()
			//{
			//	if (m_spriteBatch != nullptr)
			//	{

			//		// Begin
			//		m_spriteBatch->Begin(SpriteSortMode_Deferred, m_spriteStates->NonPremultiplied());

			//		for (const auto& image : m_images)
			//		{
			//			Vector2 size = Vector2(image.second.texture->GetWidth(), image.second.texture->GetHeight());
			//			m_spriteBatch->Draw(image.second.texture->GetResourceView(), image.second.position * m_viewport, nullptr, image.second.color,
			//				image.second.rotation, image.second.origin * size, image.second.scale * m_viewportScale);
			//		}

			//		for (const auto& text : m_texts)
			//		{
			//			text.second.font->DrawGUIText(m_spriteBatch.get(), text.second.text, text.second.position * m_viewport, text.second.scale * m_viewportScale,
			//				text.second.origin, text.second.color, text.second.rotation);
			//		}

			//		// End
			//		m_spriteBatch->End();
			//	}
			//}

			void GUIManager::Render()
			{
				if (m_spriteBatch != nullptr)
				{
					m_spriteBatch->Begin(SpriteSortMode_Deferred, m_spriteStates->NonPremultiplied());

					for (int i = 0; i < m_GUIElements.size(); ++i)
					{
						m_GUIElements[i]->Draw(m_spriteBatch.get(), m_viewport, m_viewportScale);
					}

					m_spriteBatch->End();
				}
			}

			void GUIManager::SetViewportScale(math::Viewport viewport)
			{
				m_viewport = Vector2(viewport.width, viewport.height);
				m_viewportScale = m_viewport / Vector2(1920.f, 1080.f);	//1080p because it is our standard resolution. And textures are designed for that resolution.
			}

			GUIElement* GUIManager::AddGUIElement(const std::string& text)
			{
				Font* font = m_defaultFont.get();
				Text* newText = new Text(font, text, Vector2(0.f), Vector2(1.f), Vector2(0.f), Vector4(1.f), 0, false);
				m_GUIElements.push_back(newText);

				return newText;
			}

			GUIElement* GUIManager::AddGUIElement(Texture2D * texture)
			{
				if (texture->GetResourceView())
				{
					Image* image = new Image(texture, Vector2(0.f), Vector2(1.f), Vector2(0.f), Vector4(1.f), 0, false);
					m_GUIElements.push_back(image);

					return image;
				}

				return nullptr;
			}

			void GUIManager::DeleteGUIElement(GUIElement* _element)
			{
				auto element = m_GUIElements.begin();

				while (*element._Ptr != _element && element != m_GUIElements.end())
					element++;

				if (element != m_GUIElements.end())
				{
					m_GUIElements.erase(element);
				}
			}


			//// Images
			//void GUIManager::AddImage(const std::string& id, Texture2D* texture, const Vector2& position, bool interact,
			//						  const Vector4& color, const Vector2& scale, float rotation)
			//{
			//	if (texture->GetResourceView())
			//	{
			//		Image image { texture, interact , position, scale, Vector2(0,0), color, rotation};
			//		m_images.insert(std::make_pair(id, image));
			//	}
			//}

			//void GUIManager::SetImageTexture(const std::string& id, Texture2D * texture)
			//{
			//	auto& image = GetImage(id);
			//	image.texture = texture;
			//}

			//void GUIManager::SetImagePosition(const std::string& id, const Vector2& position)
			//{
			//	auto& image = GetImage(id);
			//	image.position = position;
			//}

			//void GUIManager::SetImageColor(const std::string& id, const Vector4& color)
			//{
			//	auto& image = GetImage(id);
			//	image.color = color;
			//}

			//void GUIManager::SetImageScale(const std::string& id, const Vector2& scale)
			//{
			//	auto& image = GetImage(id);
			//	image.scale = scale;
			//}

			//void GUIManager::SetImageRotation(const std::string& id, float rotation)
			//{
			//	auto& image = GetImage(id);
			//	image.rotation = rotation;
			//}

			//void GUIManager::SetImageInteract(const std::string& id, bool interact)
			//{
			//	auto& image = GetImage(id);
			//	image.interact = interact;
			//}

			//void GUIManager::SetImageOrigin(const std::string& id, const Vector2& origin)
			//{
			//	auto& image = GetImage(id);
			//	image.origin = origin;
			//}

			//bool GUIManager::OnImageClicked(const std::string& id)
			//{
			//	thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
			//	if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
			//		return false;

			//	if (window->GetInput()->GetMouseButtonDown(Input::MouseButtons::LEFT))
			//	{
			//		return CheckImageIntersection(id);
			//	}

			//	return false;
			//}

			//bool GUIManager::OnImageHovered(const std::string& id)
			//{
			//	return CheckImageIntersection(id);
			//}

			//void GUIManager::DeleteImage(const std::string & id)
			//{
			//	m_images.erase(id);
			//}

			//GUIManager::Image& GUIManager::GetImage(const std::string& id)
			//{
			//	auto found = m_images.find(id);

			//	if (found == m_images.end()) {
			//		LOG("Image id does not exist");
			//		return Image();
			//	}
			//	return found->second;
			//}

			//bool GUIManager::CheckImageIntersection(const std::string& id)
			//{
			//	thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
			//	if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
			//		return false;

			//	if (!m_images.empty())
			//	{
			//		auto image = GetImage(id);

			//		if (image.interact)
			//		{
			//			// Construct boundaries
			//			// Note: If origin has to be changed from (0, 0) this also has to be taken into account when constructing the boundaries!
			//			Vector2 mousePos = window->GetInput()->GetMousePosition();
			//			GUIRect rect = { image.position.x * m_viewport.x,
			//						  image.position.x * m_viewport.x + image.texture->GetWidth() * image.scale.x * m_viewportScale.x,
			//						  image.position.y * m_viewport.y,
			//						  image.position.y * m_viewport.y + image.texture->GetHeight() * image.scale.y * m_viewportScale.y};

			//			if ((mousePos.x >= rect.left && mousePos.x <= rect.right) && (mousePos.y <= rect.down && mousePos.y >= rect.up))
			//			{
			//				return true;
			//			}
			//			else
			//			{
			//				return false;
			//			}
			//		}
			//	}

			//	return false;
			//}

			//// Text
			//GUIManager::Text& GUIManager::GetText(const std::string& id)
			//{
			//	auto found = m_texts.find(id);

			//	if (found == m_texts.end()) {
			//		LOG("text id does not exist");
			//		return Text();
			//	}
			//	return found->second;
			//}

			//void GUIManager::AddText(const std::string& id, const std::string& text, const Vector2& position,
			//	const Vector2& scale, float rotation, const Vector4& color, Font* font)
			//{
			//	if (font == nullptr)
			//	{
			//		font = m_defaultFont.get();
			//	}

			//	Text newText = { font, text, position, scale, Vector2(0, 0), color, rotation };
			//	m_texts.insert(std::make_pair(id, newText));
			//	m_GUIElements.push_back(newText);
			//}

			//void GUIManager::SetText(const std::string& id, const std::string& newText)
			//{
			//	auto& text = GetText(id);
			//	text.text = newText;
			//}

			//void GUIManager::SetTextPosition(const std::string& id, const Vector2& position)
			//{
			//	auto& text = GetText(id);
			//	text.position = position;
			//}

			//void GUIManager::SetTextColor(const std::string& id, const Vector4& color)
			//{
			//	auto& text = GetText(id);
			//	text.color = color;
			//}

			//void GUIManager::SetTextScale(const std::string& id, const Vector2& scale)
			//{
			//	auto& text = GetText(id);
			//	text.scale = scale;
			//}

			//void GUIManager::SetTextRotation(const std::string& id, float rotation)
			//{
			//	auto& text = GetText(id);
			//	text.rotation = rotation;
			//}
			//void GUIManager::SetFont(const std::string & id, Font* font)
			//{
			//	auto& text = GetText(id);
			//	text.font = font;
			//}
			//void GUIManager::SetTextOrigin(const std::string& id, const Vector2& origin)
			//{
			//	auto& text = GetText(id);
			//	text.origin = origin;
			//}

			//Vector2 GUIManager::GetTextSize(const std::string& id)
			//{
			//	auto& text = GetText(id);
			//	return text.font->GetTextSize(text.text);
			//}

			//void  GUIManager::DeleteText(const std::string& id)
			//{
			//	m_texts.erase(id);
			//}
		}
	}
}