#pragma once

// C++
#include <map>

// DirectXTK
#include <DirectXTK/SpriteBatch.h>

// Thomas
#include "../../resource/texture/Texture2D.h"
#include "../../resource/Font.h"
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
				bool interact;
				bool intersected;
			};

			struct Text
			{
				Text() = default;

				Font* font;
				std::string text;
				thomas::math::Vector2 position;
				thomas::math::Vector2 scale;
				thomas::math::Vector4 color;
				float rotation;
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
			static void Init();
			static void Destroy();
			static void Update();
			static void UpdateInteraction();
			static void Render();

		public:
			static void AddImage(const std::string& id, Texture2D* texture, const Vector2& position, bool interact, 
								 const Vector4& color = Vector4(1.f), const Vector2& scale = Vector2(1.f), float rotation = 0.f);
			static void SetImageTexture(const std::string& id, Texture2D* texture);
			static void SetImagePosition(const std::string& id, const Vector2& position);
			static void SetImageColor(const std::string& id, const Vector4& color);
			static void SetImageScale(const std::string& id, const Vector2& scale);
			static void SetImageRotation(const std::string& id, float rotation);
			static void SetImageInteract(const std::string& id, bool interact);
			static bool OnImageClicked(const std::string& id);
			static bool OnImageHovered(const std::string& id);

			static void AddText(const std::string& id, std::string text, const Vector2& position, const Vector2& scale = Vector2(1.f),
									float rotation = 0.f, const Vector4& color = Vector4(1.f), Font* font = s_defaultFont.get());
			static void SetTextPosition(const std::string& id, const Vector2& position);
			static void SetTextColor(const std::string& id, const Vector4& color);
			static void SetTextScale(const std::string& id, const Vector2& scale);
			static void SetTextRotation(const std::string& id, float rotation);
			
		private:
			static Image& GetImage(const std::string& id);
			static Text& GetText(const std::string& id);

		private:
			static std::map<std::string, Text> m_texts;
			static std::map<std::string, Image> m_images;
			static std::unique_ptr<SpriteBatch> m_spriteBatch;
			static std::map<std::wstring, std::unique_ptr<SpriteFont>> m_fonts;
			static std::unique_ptr<Font> s_defaultFont;

		};
	}
}
