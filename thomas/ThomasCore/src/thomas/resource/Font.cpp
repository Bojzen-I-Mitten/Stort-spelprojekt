#include "Font.h"
#include "../utils/d3d.h"
#include "../utils/Utility.h"

namespace thomas
{
	namespace resource
	{
		Font::Font(std::string path) : Resource(path)
		{
			LoadFont(path);
		}

		void Font::DrawGUIText(DirectX::SpriteBatch* spritebatch, const wchar_t* text, thomas::math::Vector2 position, thomas::math::Vector2 scale, thomas::math::Vector4 color, float rotation)
		{
			m_font->DrawString(spritebatch, text, position, color, rotation, thomas::math::Vector2(m_font->MeasureString(text)) / 2.f, scale);
		}

		DirectX::SpriteFont * Font::GetFont()
		{
			return m_font.get();
		}

		void Font::LoadFont(std::string path)
		{
			auto convertedPath = thomas::utility::ToWChar(path).c_str();
			m_font = std::make_unique<DirectX::SpriteFont>(utils::D3D::Instance()->GetDevice(), convertedPath);
		}
		void Font::OnChanged()
		{
			m_font.release();
			m_font = std::unique_ptr<DirectX::SpriteFont>();
		}
		
		
	}
}