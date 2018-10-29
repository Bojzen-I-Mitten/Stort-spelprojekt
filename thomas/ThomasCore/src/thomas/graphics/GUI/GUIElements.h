#pragma once

// Thomas
#include "../../resource/texture/Texture2D.h"
#include "../../utils/Math.h"
#include "../../resource/Font.h"
#include "../../Window.h"
#include "../../WindowManager.h"

namespace thomas
{
	using namespace resource;
	using namespace math;
	namespace graphics
	{
		namespace GUI
		{
			struct GUIRect
			{
				GUIRect() = default;
				GUIRect(float left, float right, float up, float down) : left(left), right(right), up(up), down(down) {}

				float left;
				float right;
				float up;
				float down;

				bool Intersect(Vector2 point)
				{
					return point.x > left && point.x < right && point.y > up && point.y < down;
				}
			};

			struct GUIElement
			{
				GUIElement() = default;
				GUIElement(Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation, bool interactable) :
					position(position), scale(scale), origin(origin), color(color), rotation(rotation), interactable(interactable) {effect = DirectX::SpriteEffects::SpriteEffects_None;}
				virtual ~GUIElement(){}

				Vector2 position;
				Vector2 scale;
				Vector2 origin;
				Vector4 color;
				float rotation;
				bool interactable;
				DirectX::SpriteEffects effect;

				virtual void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale) = 0;
				virtual bool OnHover(Vector2 vp, Vector2 vpScale) = 0;
				virtual bool OnClick(Vector2 vp, Vector2 vpScale) = 0;
			};

			struct Text : public GUIElement
			{
				Text() = default;
				Text(Font* font, std::string text, Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation, bool interactable) :
					font(font), text(text), GUIElement(position, scale, origin, color, rotation, interactable) {}
				virtual ~Text() {}

				Font* font;
				std::string text;

				void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale)
				{
					font->DrawGUIText(sb, text, Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height), scale * vpScale, origin, color, rotation, effect);
				}

				bool OnHover(Vector2 vp, Vector2 vpScale)
				{
					thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
					if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
						return false;
					Vector2 textSize = GetTextSize();
					GUIRect rect{ position.x * vp.x, position.x * vp.x + textSize.x * scale.x * vpScale.x,
									 position.y * vp.y, position.y * vp.y + textSize.y * scale.y * vpScale.y };
					return rect.Intersect(window->GetInput()->GetMousePosition());
				}

				bool OnClick(Vector2 vp, Vector2 vpScale)
				{
					if (interactable)
						return OnHover(vp, vpScale);
					else
						return false;
				}

				Vector2 GetTextSize()
				{
					return font->GetTextSize(text);
				}
			};

			struct Image : public GUIElement
			{
				Image() = default;
				Image(Texture2D* texture, Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation, bool interactable) :
					texture(texture), GUIElement(position, scale, origin, color, rotation, interactable) {}
				virtual ~Image() {}

				Texture2D* texture;

				void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale)
				{
					Vector2 size = Vector2(texture->GetWidth(), texture->GetHeight());
					sb->Draw(texture->GetResourceView(), Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height), nullptr, color, rotation, size, scale * vpScale, effect);
				}

				bool OnHover(Vector2 vp, Vector2 vpScale)
				{
					thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
					if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
						return false;

					GUIRect rect{ position.x * vp.x, position.x * vp.x + texture->GetWidth() * scale.x * vpScale.x,
									 position.y * vp.y, position.y * vp.y + texture->GetHeight() * scale.y * vpScale.y };
					return rect.Intersect(window->GetInput()->GetMousePosition());
				}

				bool OnClick(Vector2 vp, Vector2 vpScale)
				{
					if (interactable)
						return OnHover(vp, vpScale);
					else
						return false;
				}
			};
		}
	}
}