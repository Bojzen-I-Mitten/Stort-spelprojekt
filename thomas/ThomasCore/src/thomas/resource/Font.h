#pragma once

// DirectXTK
#include <DirectXTK/SpriteFont.h>
#include <DirectXTK/SpriteBatch.h>

// Thomas
#include "../utils/Math.h"
#include "Resource.h"

using namespace DirectX;

namespace thomas
{
	using namespace math;

	namespace resource
	{
		class Font : public Resource
		{
		public:
			Font(const std::string& path);

		public:
			void DrawGUIText(SpriteBatch* spritebatch, const std::string& text, const Vector2& position, const Vector4& color, 
				const Vector2& origin, const Vector2& scale, float rotation, DirectX::SpriteEffects effect, const float depth);
			SpriteFont* GetFont() const;
			Vector2 GetTextSize(const std::string& text);

		public:
			virtual void OnChanged() override;

		private:
			std::unique_ptr<SpriteFont> m_font;	
		};
	}
}
