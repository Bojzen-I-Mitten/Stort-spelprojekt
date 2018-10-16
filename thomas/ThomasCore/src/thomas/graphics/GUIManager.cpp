#include "GUIManager.h"

// Thomas
#include "../utils/d3d.h"
#include "../WindowManager.h"

namespace thomas
{
	namespace graphics
	{
		GUIManager::GUIManager()
		{
			m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
		}

		void GUIManager::Destroy()
		{
			m_images.clear();
			m_spriteBatch.reset();
		}

		void GUIManager::Render()
		{
			// Begin
			m_spriteBatch->Begin();

			for (const auto& image : m_images)
			{
				m_spriteBatch->Draw(image.second.texture->GetResourceView(), image.second.position, nullptr, image.second.color,
									image.second.rotation, Vector2(0.f, 0.f), image.second.scale);
			}

			// End
			m_spriteBatch->End();
		}

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
	}
}