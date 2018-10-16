#pragma once

// C++
#include <map>

// DirectXTK
#include <DirectXTK/SpriteBatch.h>

// Thomas
#include "../resource/texture/Texture2D.h"
#include "../utils/Math.h"

namespace thomas
{
	using namespace DirectX;
	using namespace resource;
	using namespace math;

	namespace graphics
	{
		class GUIManager
		{
		public:
			struct Image
			{
				Image() = default;

				Texture2D* texture;
				Vector2 position;
				Vector2 scale;
				Vector4 color;
				float rotation;
				bool interact;
			};

			struct Rect
			{
				Rect() = default;

				float left;
				float right;
				float top;
				float down;
			};

		public:
			GUIManager();
			~GUIManager() = default;
			void Destroy();
			void Render();

		public:
			void AddImage(const std::string& id, Texture2D* texture, const Vector2& position, bool interact,
						  const Vector4& color = Vector4(1.f), const Vector2& scale = Vector2(1.f), float rotation = 0.f);
			void SetImageTexture(const std::string& id, Texture2D* texture);
			void SetImagePosition(const std::string& id, const Vector2& position);
			void SetImageColor(const std::string& id, const Vector4& color);
			void SetImageScale(const std::string& id, const Vector2& scale);
			void SetImageRotation(const std::string& id, float rotation);
			void SetImageInteract(const std::string& id, bool interact);
			bool OnImageClicked(const std::string& id);
			bool OnImageHovered(const std::string& id);

		private:
			Image& GetImage(const std::string& id);
			bool CheckImageIntersection(const std::string& id);

		private:
			std::map<std::string, Image> m_images;
			std::unique_ptr<SpriteBatch> m_spriteBatch;
		};
	}
}