#include "Font.h"

// Thomas
#include "../utils/d3d.h"
#include "../utils/Utility.h"

namespace thomas
{
	namespace resource
	{
		Font::Font(const std::string& path) : 
		Resource(path), 
		m_font(nullptr)
		{
			m_font = std::make_unique<SpriteFont>(utils::D3D::Instance()->GetDevice(), utility::ToWChar(path).c_str());
		}

		void Font::DrawGUIText(SpriteBatch* spritebatch, const std::string& text, const Vector2& position,
							   const Vector2& scale, const Vector4& color, float rotation)
		{
			m_font->DrawString(spritebatch, utility::ToWChar(text).c_str(), position, color, rotation, Vector2(0.f), scale);
		}

		SpriteFont* Font::GetFont() const
		{
			return m_font.get();
		}

		void Font::OnChanged()
		{
			m_font.release();
			m_font = std::unique_ptr<SpriteFont>();
		}
	}
}