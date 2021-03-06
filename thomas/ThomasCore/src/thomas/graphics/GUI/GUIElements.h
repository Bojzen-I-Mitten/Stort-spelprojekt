#pragma once

#include "../../resource/texture/Texture2D.h"
#include "../../utils/Math.h"
#include "../../resource/Font.h"
#include "../../Window.h"
#include "../../WindowManager.h"
#include "Canvas.h"
#include "../../Common.h"
#include "../../ThomasCore.h"

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
				GUIElement(Canvas* canvas) : canvas(canvas)
				{
					position = Vector2::Zero;
					scale = Vector2::One;
					origin = Vector2::Zero;
					color = Vector4::One;
					rotation = 0;
					interactable = false;
					rendering = true;
					depth = 0;
					effect = DirectX::SpriteEffects::SpriteEffects_None;
				}
				virtual ~GUIElement(){ }

				Vector2 position;
				Vector2 scale;
				Vector2 origin;
				Vector4 color;
				float rotation;
				bool interactable;
				bool rendering;
				float depth;
				Canvas* canvas;
				DirectX::SpriteEffects effect;

				virtual void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale) = 0;
				virtual Vector2 PixelSize() = 0;
				Vector2 Size()
				{
					return (PixelSize() * scale * canvas->GetViewportScale()) / Vector2(canvas->GetViewport().width, canvas->GetViewport().height);
				}

				bool Hovered()
				{
					thomas::Window* window = WindowManager::Instance()->GetCurrentBound();
					if (!window ||
						WindowManager::Instance()->GetCurrentBound() == WindowManager::Instance()->GetEditorWindow() ||
						!canvas->GetRendering() ||
						!interactable ||
						!rendering)
						return false;

					Viewport canvasViewport = canvas->GetViewport();
					Vector2 vpScale = canvas->GetViewportScale();
					Vector2 size = PixelSize();
					size *= scale * vpScale;
					float left = position.x * canvasViewport.width - size.x * origin.x;
					float right = left + size.x;
					float top = position.y * canvasViewport.height - size.y * origin.y;
					float bottom = top + size.y;
					GUIRect rect{ left, right, top, bottom };

					return rect.Intersect(window->GetInput()->GetMousePosition());
				}

				bool Clicked()
				{
					if (WindowManager::Instance()->GetGameInput()->GetMouseButtonUp(Input::MouseButtons::LEFT))
						return Hovered();
					else
						return false;
				}
			};

			struct Text : public GUIElement
			{
				Text() = default;
				Text(Font* font, std::string text, Canvas* canvas) :
					font(font), text(text), GUIElement(canvas)
				{
					outline = false;
					outlineColor = Vector4(0.0f, 0.0f, 0.0f, 1);
				}
				
				virtual ~Text() {}

				Font* font;
				std::string text;
				bool outline;
				Vector4 outlineColor;

				void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale)
				{
					if (rendering)
					{
						if (outline)
						{
							font->DrawGUIText(sb, text, (Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height)) + Vector2( 1.0f,  1.0f), outlineColor, origin * PixelSize(), (scale + Vector2(0.01f)) * vpScale, rotation, effect, depth);
							font->DrawGUIText(sb, text, (Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height)) + Vector2(-1.0f,  1.0f), outlineColor, origin * PixelSize(), (scale + Vector2(0.01f)) * vpScale, rotation, effect, depth);
							font->DrawGUIText(sb, text, (Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height)) + Vector2(-1.0f, -1.0f), outlineColor, origin * PixelSize(), (scale + Vector2(0.01f)) * vpScale, rotation, effect, depth);
							font->DrawGUIText(sb, text, (Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height)) + Vector2( 1.0f, -1.0f), outlineColor, origin * PixelSize(), (scale + Vector2(0.01f)) * vpScale, rotation, effect, depth);
						}

						font->DrawGUIText(sb, text, Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height), color, origin * PixelSize(), scale * vpScale, rotation, effect, depth);
					}
				}

				Vector2 PixelSize()
				{
					return font->GetTextSize(text);
				}
			};

			struct Image : public GUIElement
			{
				Image() = default;
				Image(Texture2D* texture, Canvas* canvas) :
					texture(texture), GUIElement(canvas) {}
				virtual ~Image() {}

				Texture2D* texture;

				void Draw(SpriteBatch* sb, Viewport vp, Vector2 vpScale)
				{
					if(rendering)
						sb->Draw(texture->GetResourceView(), Vector2(vp.x, vp.y) + position * Vector2(vp.width, vp.height), nullptr, color, rotation, origin * PixelSize(), scale * vpScale, effect, depth);
				}

				Vector2 PixelSize()
				{
					return Vector2(float(texture->GetWidth()), float(texture->GetHeight()));
				}
			};
		}
	}
}