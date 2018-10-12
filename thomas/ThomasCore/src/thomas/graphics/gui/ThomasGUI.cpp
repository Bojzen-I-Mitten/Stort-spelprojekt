#include "ThomasGUI.h"

// Thomas
#include "../../utils/d3d.h"

namespace thomas
{
	namespace GUI
	{
		std::vector<ThomasGUI::Image> ThomasGUI::m_images;
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

			for (const auto& e : m_images)
			{
				m_spriteBatch->Draw(e.texture->GetResourceView(), e.position, nullptr, e.color, e.rotation,
									Vector2(e.texture->GetWidth() / 2.f, e.texture->GetHeight() / 2.f), e.scale);
			}

			//End
			m_spriteBatch->End();
		}

		void ThomasGUI::Update()
		{
			m_images.clear();
		}

		void ThomasGUI::AddImage(Texture2D* texture, const Vector2& position, const Vector4& color,
								 const Vector2& scale, float rotation)
		{
			if (texture->GetResourceView())
			{
				m_images.push_back({ texture, position, scale, color, rotation });
			}
		}
	}
}