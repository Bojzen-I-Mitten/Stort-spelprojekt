#pragma once

// DirectXTK
#include <DirectXTK/SpriteBatch.h>

// Thomas
#include "../../resource/texture/Texture2D.h"
#include "../../utils/Math.h"

namespace thomas
{
	using namespace DirectX;
	using namespace resource;
	using namespace math;

	class Sprite
	{
	public:
		static void Draw(SpriteBatch* batch, Texture2D* texture, const Vector2& position, const Vector2& scale = Vector2(1.f), 
						 float rotation = 0.f, const Vector4& color = Vector4(1.f));
	};
}
