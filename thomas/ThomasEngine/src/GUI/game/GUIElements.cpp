#pragma unmanaged
#pragma managed
#include "GUIElements.h"
#include "../../Utility.h"
#include "../../resource/Font.h"
#include "../../resource/texture/Texture2D.h"

namespace ThomasEngine
{
	void GUIElement::position::set(Vector2 position) { nativePtr->position = Utility::Convert(position); }

	void GUIElement::scale::set(Vector2 scale) { nativePtr->scale = Utility::Convert(scale); }

	void GUIElement::origin::set(Vector2 origin) { nativePtr->origin = Utility::Convert(origin); }

	void GUIElement::color::set(Vector4 color) { nativePtr->color = Utility::Convert(color); }

	void GUIElement::interactable::set(bool interactable) { ((thomas::graphics::GUI::Image*)nativePtr)->interactable = interactable; }


	Text::Text(thomas::graphics::GUI::GUIElement* ptr) { nativePtr = ptr; }

	void Text::text::set(String^ text) { ((thomas::graphics::GUI::Text*)nativePtr)->text = Utility::ConvertString(text); }

	void Text::font::set(Font^ font) { ((thomas::graphics::GUI::Text*)nativePtr)->font = (thomas::resource::Font*)(font->m_nativePtr); }


	Image::Image(thomas::graphics::GUI::GUIElement* ptr) { nativePtr = ptr; }

	void Image::texture::set(Texture2D^ texture) { ((thomas::graphics::GUI::Image*)nativePtr)->texture = (thomas::resource::Texture2D*)(texture->m_nativePtr);}
}