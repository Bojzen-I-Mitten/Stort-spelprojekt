#include "ThomasGUI.h"

// Thomas
#include "../../utils/d3d.h"
#include "../../WindowManager.h"

namespace thomas
{
	namespace GUI
	{
		std::map<std::string, ThomasGUI::Image> ThomasGUI::m_images;
		std::unique_ptr<SpriteBatch> ThomasGUI::m_spriteBatch;

		void ThomasGUI::Init()
		{
			m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
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

			// End
			m_spriteBatch->End();
		}

		void ThomasGUI::Update()
		{
			m_images.clear();
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
	}
}