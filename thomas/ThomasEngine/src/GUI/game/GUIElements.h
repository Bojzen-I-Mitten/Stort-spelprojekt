#pragma once
#pragma unmanaged
#include <thomas/graphics/GUI/GUIElements.h>
#pragma managed
#include "Canvas.h"
using namespace System;


namespace ThomasEngine
{
	ref class Font;
	ref class Texture2D;
	public ref class GUIElement
	{
	public:
		thomas::graphics::GUI::GUIElement* nativePtr; 
	public:
		GUIElement() {}

		property Vector2 position
		{
			void set(Vector2 position);
		}
		property Vector2 scale
		{
			void set(Vector2 scale);
		}
		property Vector2 origin
		{
			void set(Vector2 origin);
		}
		property Vector4 color
		{
			void set(Vector4 color);
		}
		property float rotation
		{
			void set(float rotation);
		}
		property bool interactable
		{
			void set(bool interactable);
		}
		property Vector2 flip
		{
			void set(Vector2 Flip);
		}
		property Vector2 size //Note that Image size returns Vector2::Zero
		{
			Vector2 get();
		}

		bool Clicked();
		bool Hovered();
	};

	public ref class Text : public GUIElement
	{
	public:
		Text(thomas::graphics::GUI::GUIElement* ptr);

		property String^ text
		{
			void set(String^ text);
		}
		property Font^ font
		{
			void set(Font^ font);
		}
	};

	public ref class Image : public GUIElement
	{
	public:
		Image(thomas::graphics::GUI::GUIElement* ptr);

		property Texture2D^ texture
		{
			void set(Texture2D^ texture);
		}
	};
}