#pragma once

// DirectXTK
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/SpriteFont.h>

// Thomas
#include "../../resource/texture/Texture2D.h"
#include "../../utils/Math.h"

#include <map>

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
			struct Text
			{
				Text() = default;

				std::wstring font;
				wchar_t* text;
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
			static void AddFont(std::wstring font);

		public:
			static void AddImage(Texture2D* texture, const Vector2& position, const Vector2& scale = Vector2(1.f),
								 float rotation = 0.f, const Vector4& color = Vector4(1.f));

			static void AddText(wchar_t* text, std::wstring font, const Vector2& position, const Vector2& scale = Vector2(1.f),
				float rotation = 0.f, const Vector4& color = Vector4(1.f));


		private:
			static std::vector<Image> m_images;
			static std::vector<Text> m_texts;
			static std::unique_ptr<SpriteBatch> m_spriteBatch;
			static std::map<std::wstring, std::unique_ptr<SpriteFont>> m_fonts;

		};
	}
}
