#pragma once

#include <string>
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/SpriteBatch.h>
#include "../utils/Math.h"
#include "Resource.h"

namespace thomas
{
	namespace resource
	{
		class Font : public Resource
		{
		public:
			Font(std::string path);
			void DrawGUIText(DirectX::SpriteBatch* spritebatch, std::string text, thomas::math::Vector2 position, thomas::math::Vector2 scale, thomas::math::Vector4 color, float rotation);
			DirectX::SpriteFont* GetFont();

		private:
			void LoadFont(std::string path);
			void OnChanged();

		private:
			std::unique_ptr<DirectX::SpriteFont> m_font;
			
		};
	}
}
