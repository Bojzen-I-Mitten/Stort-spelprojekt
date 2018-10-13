#pragma once

// C++
#include <map>

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

	namespace GUI
	{
		class ThomasGUI
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
			};

		public:
			static void Init();
			static void Destroy();
			static void Update();
			static void Render();

		public:
			static void AddImage(const std::string& id, Texture2D* texture, const Vector2& position, const Vector4& color = Vector4(1.f),
								 const Vector2& scale = Vector2(1.f), float rotation = 0.f);
			static void SetImageTexture(const std::string& id, Texture2D* texture);
			static void SetImagePosition(const std::string& id, const Vector2& position);
			static void SetImageColor(const std::string& id, const Vector4& color);
			static void SetImageScale(const std::string& id, const Vector2& scale);
			static void SetImageRotation(const std::string& id, float rotation);
			

		private:
			static Image& GetImage(const std::string& id);

		private:
			static std::map<std::string, Image> m_images;
			static std::unique_ptr<SpriteBatch> m_spriteBatch;
		};
	}
}
