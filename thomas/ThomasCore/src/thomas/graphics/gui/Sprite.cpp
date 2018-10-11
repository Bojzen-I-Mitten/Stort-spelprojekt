#include "Sprite.h"

namespace thomas
{
	void Sprite::Draw(SpriteBatch* batch, Texture2D* texture, const Vector2& position, const Vector2& scale, 
					  float rotation, const Vector4& color)
	{
		// Origin will always be in the middle of the sprite
		batch->Draw(texture->GetResourceView(), position, nullptr, color, rotation, 
					Vector2(texture->GetWidth() / 2.f, texture->GetHeight() / 2.f), scale);
	}
}