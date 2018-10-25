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
			void DrawGUIText(SpriteBatch* spritebatch, const std::string& text, const Vector2& position, const Vector2& scale, 
							 const Vector4& color, float rotation);
			SpriteFont* GetFont() const;

		public:
			virtual void OnChanged() override;

		private:
			std::unique_ptr<SpriteFont> m_font;	
		};
	}
}
