#pragma once

// C++
#include <map>

// DirectXTK
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/CommonStates.h>

// Thomas
#include "../resource/texture/Texture2D.h"
#include "../utils/Math.h"
#include "../resource/Font.h"

using namespace DirectX;

namespace thomas
{
	using namespace resource;
	using namespace math;

	namespace graphics
	{
		class GUIManager
		{
		public:
			struct GUIElement
			{
				GUIElement() = default;
				GUIElement(Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation) :
					position(position), scale(scale), origin(origin), color(color), rotation(rotation) {}

				Vector2 position;
				Vector2 scale;
				Vector2 origin;
				Vector4 color;
				float rotation;

				virtual void Draw(SpriteBatch* sb, Vector2 vp, Vector2 vpScale) = 0;
			};

			struct Text : public GUIElement
			{
				Text() = default;
				Text(Font* font, std::string text, Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation) :
					font(font), text(text), GUIElement(position, scale, origin, color, rotation) {}

				Font* font;
				std::string text;

				void Draw(SpriteBatch* sb, Vector2 vp, Vector2 vpScale)
				{
					font->DrawGUIText(sb, text, position * vp, scale * vpScale, origin, color, rotation);
				}

				Vector2 GetTextSize()
				{
					font->GetTextSize(text);
				}
			};

			struct Image : public GUIElement
			{
				Image() = default;
				Image(Texture2D* texture, bool interact, Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation) :
					texture(texture), interact(interact), GUIElement(position, scale, origin, color, rotation) {}

				Texture2D* texture;
				bool interact;

				void Draw(SpriteBatch* sb, Vector2 vp, Vector2 vpScale)
				{
					Vector2 size = Vector2(texture->GetWidth(), texture->GetHeight());
					sb->Draw(texture->GetResourceView(), position * vp, nullptr, color, rotation, size, scale * vpScale);
				}

				bool OnClick(Vector2 vp, Vector2 vpScale)
				{
					thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
					if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
						return false;

					return OnInteract(window, vp, vpScale);
				}
				bool OnHover(Vector2 vp, Vector2 vpScale)
				{
					thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
					if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
						return false;

					return OnInteract(window, vp, vpScale);
				}

				bool OnInteract(thomas::Window* window, Vector2 vp, Vector2 vpScale)
				{
					Rect rect{ position.x * vp.x, position.x * vp.x + texture->GetWidth() * scale.x * vpScale.x,
									 position.y * vp.y, position.y * vp.y + texture->GetHeight() * scale.y * vpScale.y };
					return rect.Intersect(window->GetInput->GetMousePosition());
				}
			};

			struct Rect
			{
				Rect() = default;
				Rect(float left, float right, float up, float down) : left(left), right(right), up(up), down(down) {}

				float left;
				float right;
				float up;
				float down;

				bool Intersect(Vector2 point)
				{
					return point.x > left && point.x < right && point.y > up && point.y < down;
				}
			};

		public:
			GUIManager();
			~GUIManager() = default;
			void Destroy();
			void Render();

			void NewRender();

		public:
			void SetViewportScale(math::Viewport viewport);
			GUIElement* AddGUIElement(const std::string & id, const std::string & text, const Vector2 & position, const Vector2 & scale, float rotation, const Vector4 & color, Font * font);
			void DeleteGUIElement(GUIElement* element);

			// Images
			void AddImage(const std::string& id, Texture2D* texture, const Vector2& position, bool interact,
						  const Vector4& color = Vector4(1.f), const Vector2& scale = Vector2(1.f), float rotation = 0.f);
			void SetImageTexture(const std::string& id, Texture2D* texture);
			void SetImagePosition(const std::string& id, const Vector2& position);
			void SetImageColor(const std::string& id, const Vector4& color);
			void SetImageScale(const std::string& id, const Vector2& scale);
			void SetImageRotation(const std::string& id, float rotation);
			void SetImageInteract(const std::string& id, bool interact);
			void SetImageOrigin(const std::string& id, const Vector2& origin);
			bool OnImageClicked(const std::string& id);
			bool OnImageHovered(const std::string& id);
			void DeleteImage(const std::string& id);

			// Text
			void AddText(const std::string& id, const std::string& text, const Vector2& position, const Vector2& scale = Vector2(1.f),
						 float rotation = 0.f, const Vector4& color = Vector4(0.f, 0.f, 0.f, 1.f), Font* font = nullptr);
			void SetText(const std::string& id, const std::string& newText);
			void SetTextPosition(const std::string& id, const Vector2& position);
			void SetTextColor(const std::string& id, const Vector4& color);
			void SetTextScale(const std::string& id, const Vector2& scale);
			void SetTextRotation(const std::string& id, float rotation);
			void SetFont(const std::string& id, Font* font);
			void DeleteText(const std::string& id);
			void SetTextOrigin(const std::string& id, const Vector2& origin);
			Vector2 GetTextSize(const std::string& id);
		private:
			Image& GetImage(const std::string& id);
			Text& GetText(const std::string& id);
			bool CheckImageIntersection(const std::string& id);

		private:
			std::vector<GUIElement> m_GUIElements;
			std::map<std::string, Image> m_images;
			std::map<std::string, Text> m_texts;
			std::unique_ptr<CommonStates> m_spriteStates;
			std::unique_ptr<SpriteBatch> m_spriteBatch;
			std::unique_ptr<Font> m_defaultFont;
			math::Vector2 m_viewportScale;
			math::Vector2 m_viewport;
		};
	}
}