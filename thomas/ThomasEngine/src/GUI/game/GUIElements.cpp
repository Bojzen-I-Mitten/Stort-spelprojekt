#pragma unmanaged
#pragma managed
#include "GUIElements.h"
#include "../../Utility.h"
#include "../../resource/Font.h"
#include "../../resource/texture/Texture2D.h"

namespace ThomasEngine
{
	void GUIElement::position::set(Vector2 position) { nativePtr->position = Utility::Convert(position); }

	Vector2 GUIElement::position::get() { return Utility::Convert(nativePtr->position); }

	void GUIElement::scale::set(Vector2 scale) { nativePtr->scale = Utility::Convert(scale); }

	Vector2 GUIElement::scale::get() { return Utility::Convert(nativePtr->scale); }

	void GUIElement::origin::set(Vector2 origin) { nativePtr->origin = Utility::Convert(origin); }

	void GUIElement::color::set(Color color) { nativePtr->color = Utility::Convert(color); }

	Color GUIElement::color::get() { return Color(Utility::Convert(nativePtr->color)); }

	void GUIElement::rotation::set(float rotation) { nativePtr->rotation = rotation; }

	float GUIElement::rotation::get() { return nativePtr->rotation; }

	void GUIElement::interactable::set(bool interactable) { nativePtr->interactable = interactable; }

	void GUIElement::depth::set(float depth) { nativePtr->depth = depth; }

	void GUIElement::flip::set(Vector2 flip) 
	{ 
		Vector2 _flip = Vector2(flip.x ? 2.f : 0.f, flip.y ? 1.f : 0.f);
		nativePtr->effect = (DirectX::SpriteEffects)((int)_flip.x | (int)_flip.y);
	}

	Vector2 GUIElement::size::get() { return Utility::Convert(nativePtr->Size()); }

	bool GUIElement::Clicked() { return nativePtr->Clicked(); }

	bool GUIElement::Hovered() { return nativePtr->Hovered(); }


	Text::Text(thomas::graphics::GUI::GUIElement* ptr) { nativePtr = ptr; }

	void Text::text::set(String^ text) { ((thomas::graphics::GUI::Text*)nativePtr)->text = Utility::ConvertString(text); }

	String^ Text::text::get() { return Utility::ConvertString(((thomas::graphics::GUI::Text*)nativePtr)->text); }

	void Text::font::set(Font^ font) { ((thomas::graphics::GUI::Text*)nativePtr)->font = (thomas::resource::Font*)(font->m_nativePtr); }


	Image::Image(thomas::graphics::GUI::GUIElement* ptr) { nativePtr = ptr; }

	void Image::texture::set(Texture2D^ texture) { ((thomas::graphics::GUI::Image*)nativePtr)->texture = (thomas::resource::Texture2D*)(texture->m_nativePtr);}
}