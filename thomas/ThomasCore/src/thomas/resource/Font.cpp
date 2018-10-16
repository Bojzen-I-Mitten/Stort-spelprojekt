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

		void Font::DrawGUIText(DirectX::SpriteBatch* spritebatch, std::string text, thomas::math::Vector2 position, thomas::math::Vector2 scale, thomas::math::Vector4 color, float rotation)
		{
			std::wstring convertedText = thomas::utility::ToWChar(text);
			const wchar_t* wchar = convertedText.c_str();
			m_font->DrawString(spritebatch, wchar, position, color, rotation, thomas::math::Vector2(m_font->MeasureString(wchar)) / 2.f, scale);
		}

		DirectX::SpriteFont * Font::GetFont()
		{
			return m_font.get();
		}

		void Font::LoadFont(std::string path)
		{
			std::wstring convertedPath = thomas::utility::ToWChar(path);
			const wchar_t* wchar = convertedPath.c_str();
			m_font = std::make_unique<DirectX::SpriteFont>(utils::D3D::Instance()->GetDevice(), wchar);
		}

		void Font::OnChanged()
		{
			m_font.release();
			m_font = std::unique_ptr<DirectX::SpriteFont>();
		}
	}
}