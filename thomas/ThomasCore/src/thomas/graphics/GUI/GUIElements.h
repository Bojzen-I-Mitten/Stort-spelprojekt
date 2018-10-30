#pragma once

#include "../../resource/texture/Texture2D.h"
#include "../../utils/Math.h"
#include "../../resource/Font.h"
#include "../../Window.h"
#include "../../WindowManager.h"
#include "Canvas.h"

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
				GUIElement(Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation, bool interactable, Canvas* canvas) :
					position(position), scale(scale), origin(origin), color(color), rotation(rotation), interactable(interactable), canvas(canvas) {effect = DirectX::SpriteEffects::SpriteEffects_None;}
				virtual ~GUIElement(){}

				Vector2 position;
				Vector2 scale;
				Vector2 origin;
				Vector4 color;
				float rotation;
				bool interactable;
				Canvas* canvas;
				DirectX::SpriteEffects effect;

				virtual void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale) = 0;
				virtual Vector2 Size() = 0;

				bool Hovered()
				{
					thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
					if (!window || WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow())
						return false;
					Vector2 size = Size();
					GUIRect rect{ position.x * canvas->GetViewport().width, position.x * canvas->GetViewport().width + size.x * scale.x * canvas->GetViewportScale().x,
									 position.y * canvas->GetViewport().height, position.y * canvas->GetViewport().height + size.y * scale.y * canvas->GetViewportScale().y };
					return rect.Intersect(window->GetInput()->GetMousePosition());
				}

				bool Clicked()
				{
					if (interactable)
						return Hovered();
					else
						return false;
				}
			};

			struct Text : public GUIElement
			{
				Text() = default;
				Text(Font* font, std::string text, Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation, bool interactable, Canvas* canvas) :
					font(font), text(text), GUIElement(position, scale, origin, color, rotation, interactable, canvas) {}
				virtual ~Text() {}

				Font* font;
				std::string text;

				void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale)
				{
					font->DrawGUIText(sb, text, Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height), scale * vpScale, origin, color, rotation, effect);
				}

				Vector2 Size()
				{
					return font->GetTextSize(text);
				}
			};

			struct Image : public GUIElement
			{
				Image() = default;
				Image(Texture2D* texture, Vector2 position, Vector2 scale, Vector2 origin, Vector4 color, float rotation, bool interactable, Canvas* canvas) :
					texture(texture), GUIElement(position, scale, origin, color, rotation, interactable, canvas) {}
				virtual ~Image() {}

				Texture2D* texture;

				void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale)
				{
					sb->Draw(texture->GetResourceView(), Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height), nullptr, color, rotation, origin * Size(), scale * vpScale, effect);
				}

				Vector2 Size()
				{
					return Vector2(texture->GetWidth(), texture->GetHeight());
				}
			};
		}
	}
}