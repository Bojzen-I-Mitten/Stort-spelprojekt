#pragma unmanaged
#include <thomas/object/component/GUIComponent.h>

#pragma managed
#include "GUIComponent.h"

namespace ThomasEngine
{
	GUIComponent::GUIComponent() : Component(new thomas::object::component::GUIComponent())
	{
	}

	thomas::object::component::GUIComponent* GUIComponent::handle::get() 
	{
		return (thomas::object::component::GUIComponent*)nativePtr; 
	}

	void GUIComponent::AddImage(String^ id, Texture2D^ texture, Vector2 position, bool interact)
	{
		handle->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
						 interact);
	}

	void GUIComponent::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector4 color, bool interact)
	{
		handle->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
						 interact, Utility::Convert(color));
	}

	void  GUIComponent::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, bool interact)
	{
		handle->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
						 interact, Utility::Convert(Vector4(1.f)), Utility::Convert(scale));
	}

	void GUIComponent::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, bool interact)
	{
		handle->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
						 interact, Utility::Convert(Vector4(1.f)), Utility::Convert(scale), rotation);
	}

	void GUIComponent::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, Vector4 color, bool interact)
	{
		handle->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
						 interact, Utility::Convert(color), Utility::Convert(scale), rotation);
	}

	bool GUIComponent::OnImageClicked(String^ id)
	{
		return handle->OnImageClicked(Utility::ConvertString(id));
	}

	bool GUIComponent::OnImageHovered(String^ id)
	{
		return handle->OnImageHovered(Utility::ConvertString(id));
	}

	void GUIComponent::SetImageTexture(String^ id, Texture2D ^ texture)
	{
		handle->SetImageTexture(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr);
	}

	void GUIComponent::SetImagePosition(String^ id, Vector2 position)
	{
		handle->SetImagePosition(Utility::ConvertString(id), Utility::Convert(position));
	}

	void GUIComponent::SetImageColor(String^ id, Vector4 color)
	{
		handle->SetImageColor(Utility::ConvertString(id), Utility::Convert(color));
	}

	void GUIComponent::SetImageScale(String^ id, Vector2 scale)
	{
		handle->SetImageScale(Utility::ConvertString(id), Utility::Convert(scale));
	}

	void GUIComponent::SetImageRotation(String^ id, float rotation)
	{
		handle->SetImageRotation(Utility::ConvertString(id), rotation);
	}

	void GUIComponent::SetImageInteract(String^ id, bool interact)
	{
		handle->SetImageInteract(Utility::ConvertString(id), interact);
	}
}