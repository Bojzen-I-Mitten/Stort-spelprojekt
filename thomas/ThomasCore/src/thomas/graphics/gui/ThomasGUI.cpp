#include "ThomasGUI.h"

// Thomas
#include "../../utils/d3d.h"

namespace thomas
{
	namespace GUI
	{
		std::vector<ThomasGUI::Image> ThomasGUI::m_images;
		std::vector<ThomasGUI::Text> ThomasGUI::m_texts;
		std::unique_ptr<SpriteBatch> ThomasGUI::m_spriteBatch;
		std::map<std::wstring, std::unique_ptr<SpriteFont>> ThomasGUI::m_fonts;

		void ThomasGUI::Init()
		{
			m_spriteBatch = std::make_unique<SpriteBatch>(utils::D3D::Instance()->GetDeviceContext());
			m_fonts = std::map<std::wstring, std::unique_ptr<SpriteFont>>();
			//Default font
			AddFont(L"CourierNew");
			//m_fonts.insert(std::make_pair(L"CourierNew", std::make_unique<SpriteFont>(utils::D3D::Instance()->GetDevice(), L"../Data/Fonts/CourierNew.spritefont")));
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
			for (const auto& t : m_texts)
			{
				m_fonts[t.font]->DrawString(m_spriteBatch.get(), t.text, t.position, t.color, t.rotation,
					m_fonts[t.font]->MeasureString(t.text) / 2.f);
			}

			//End
			m_spriteBatch->End();
		}

		void ThomasGUI::AddFont(std::wstring font)
		{
			std::wstring d = L"../Data/Fonts/" + font + L".spritefont";
			const wchar_t* dir = d.c_str();
			m_fonts.insert(std::make_pair(font, std::make_unique<SpriteFont>(utils::D3D::Instance()->GetDevice(), dir)));
		}

		void ThomasGUI::Update()
		{
			m_images.clear();
			m_texts.clear();
		}

		void ThomasGUI::AddImage(Texture2D* texture, const Vector2& position, const Vector2& scale,
								float rotation, const Vector4& color)
		{
			m_images.push_back({ texture, position, scale, color, rotation });
		}
		void ThomasGUI::AddText(wchar_t* text, std::wstring font, const Vector2 & position, const Vector2 & scale, float rotation, const Vector4 & color)
		{
			m_texts.push_back({ font, text, position, scale, color, rotation });
		}
	}
}