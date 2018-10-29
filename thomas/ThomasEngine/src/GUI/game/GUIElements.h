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
		property bool interactable
		{
			void set(bool interactable);
		}
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