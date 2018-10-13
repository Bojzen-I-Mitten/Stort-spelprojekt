#include "ThomasGUI.h"

// Thomas
#include "../../utils/d3d.h"

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
									image.second.rotation, Vector2(0.f, 0.f), image.second.scale);
			}

			//End
			m_spriteBatch->End();
		}

		void ThomasGUI::Update()
		{
			m_images.clear();
		}

		void ThomasGUI::AddImage(const std::string& id, Texture2D* texture, const Vector2& position, const Vector4& color,
								 const Vector2& scale, float rotation)
		{
			if (texture->GetResourceView())
			{
				Image image = { texture, position, scale, color, rotation };
				m_images.insert(std::make_pair(id, image));
			}
		}
	}
}