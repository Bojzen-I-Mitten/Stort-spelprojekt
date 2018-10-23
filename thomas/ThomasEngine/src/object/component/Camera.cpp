#pragma unmanaged
#include <thomas/object/component/Camera.h>
#include <thomas/graphics/GUIManager.h>

#pragma managed
#include "Camera.h"
#include "../../Utility.h"

namespace ThomasEngine
{
	Camera::Camera() : Component(new thomas::object::component::Camera()) { }

	thomas::object::component::Camera* Camera::camera::get(){ return (thomas::object::component::Camera*)nativePtr; }

	Matrix Camera::viewMatrix::get() { return Utility::Convert(camera->GetViewMatrix()); }
	Matrix Camera::projectionMatrix::get() { return Utility::Convert(camera->GetProjMatrix()); }

	Vector3 Camera::position::get() { return Utility::Convert(camera->GetPosition()); }

	float Camera::fieldOfView::get() { return camera->GetFov(); }
	void Camera::fieldOfView::set(float value) { camera->SetFov(value); }

	float Camera::nearPlane::get() { return camera->GetNear(); }
	void Camera::nearPlane::set(float value) { camera->SetNear(value); }

	float Camera::farPlane::get() { return camera->GetFar(); }
	void Camera::farPlane::set(float value) { camera->SetFar(value); }

	int Camera::targetDisplay::get() { return camera->GetTargetDisplayIndex(); }
	void Camera::targetDisplay::set(int value) { camera->SetTargetDisplay(value); }

	bool Camera::renderGUI::get() { return camera->GetGUIRendering(); }
	void Camera::renderGUI::set(bool value) { camera->SetGUIRendering(value); }

	Vector2 Camera::GetViewPort()
	{
		DirectX::SimpleMath::Viewport viewPort = camera->GetViewport();
		return Vector2(viewPort.width, viewPort.height);
	}

	//-------------------------------------------------------------------------------------------
	void Camera::AddImage(String^ id, Texture2D^ texture, Vector2 position, bool interact)
	{
		camera->GetGUIHandle()->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
										 interact);
	}

	void Camera::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector4 color, bool interact)
	{
		camera->GetGUIHandle()->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
										 interact, Utility::Convert(color));
	}

	void Camera::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, bool interact)
	{
		camera->GetGUIHandle()->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
										 interact, Utility::Convert(Vector4(1.f)), Utility::Convert(scale));
	}

	void Camera::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, bool interact)
	{
		camera->GetGUIHandle()->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
										 interact, Utility::Convert(Vector4(1.f)), Utility::Convert(scale), rotation);
	}

	void Camera::AddImage(String^ id, Texture2D^ texture, Vector2 position, Vector2 scale, float rotation, Vector4 color, bool interact)
	{
		camera->GetGUIHandle()->AddImage(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr, Utility::Convert(position),
										 interact, Utility::Convert(color), Utility::Convert(scale), rotation);
	}

	bool Camera::OnImageClicked(String^ id)
	{
		return camera->GetGUIHandle()->OnImageClicked(Utility::ConvertString(id));
	}

	bool Camera::OnImageHovered(String^ id)
	{
		return camera->GetGUIHandle()->OnImageHovered(Utility::ConvertString(id));
	}

	void Camera::SetImageTexture(String^ id, Texture2D^ texture)
	{
		camera->GetGUIHandle()->SetImageTexture(Utility::ConvertString(id), (thomas::resource::Texture2D*)texture->m_nativePtr);
	}

	void Camera::SetImagePosition(String^ id, Vector2 position)
	{
		camera->GetGUIHandle()->SetImagePosition(Utility::ConvertString(id), Utility::Convert(position));
	}

	void Camera::SetImageColor(String^ id, Vector4 color)
	{
		camera->GetGUIHandle()->SetImageColor(Utility::ConvertString(id), Utility::Convert(color));
	}

	void Camera::SetImageScale(String^ id, Vector2 scale)
	{
		camera->GetGUIHandle()->SetImageScale(Utility::ConvertString(id), Utility::Convert(scale));
	}

	void Camera::SetImageRotation(String^ id, float rotation)
	{
		camera->GetGUIHandle()->SetImageRotation(Utility::ConvertString(id), rotation);
	}

	void Camera::SetImageInteract(String^ id, bool interact)
	{
		camera->GetGUIHandle()->SetImageInteract(Utility::ConvertString(id), interact);
	}

	void Camera::DeleteImage(String ^ id)
	{
		camera->GetGUIHandle()->DeleteImage(Utility::ConvertString(id));
	}

	//-------------------------------------------------------------------------------------------

	void Camera::AddText(String^ id, String^ text, Vector2 position)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text), Utility::Convert(position));
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Font^ font)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text), Utility::Convert(position), 
										Utility::Convert(Vector2(1.f)), 0.f, Utility::Convert(Vector4(0.f, 0.f, 0.f, 1.f)), 
										(thomas::resource::Font*)font->m_nativePtr);
	}

	void Camera::AddText(String ^ id, String ^ text, Vector2 position, Vector2 scale, Font ^ font)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text), Utility::Convert(position),
			Utility::Convert(scale), 0.f, Utility::Convert(Vector4(0.f, 0.f, 0.f, 1.f)),
			(thomas::resource::Font*)font->m_nativePtr);
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Vector2 scale)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text), 
										Utility::Convert(position), Utility::Convert(scale));
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, float rotation)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text), 
										Utility::Convert(position), Utility::Convert(Vector2(1.f)), rotation);
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Vector4 color)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text),
										Utility::Convert(position), Utility::Convert(Vector2(1.f)), 0.f, 
										Utility::Convert(color));
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Font^ font, Vector4 color)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text), 
										Utility::Convert(position), Utility::Convert(Vector2(1.f)), 0.f, Utility::Convert(color), 
										(thomas::resource::Font*)font->m_nativePtr);
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Vector2 scale, float rotation, Vector4 color)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text),
										Utility::Convert(position), Utility::Convert(scale), rotation, Utility::Convert(color));
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Vector2 scale, Font^ font, Vector4 color)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text),
										Utility::Convert(position), Utility::Convert(scale), 0.f, Utility::Convert(color), 
										(thomas::resource::Font*)font->m_nativePtr);
	}

	void Camera::AddText(String^ id, String^ text, Vector2 position, Vector2 scale, float rotation, Font^ font, Vector4 color)
	{
		camera->GetGUIHandle()->AddText(Utility::ConvertString(id), Utility::ConvertString(text),
										Utility::Convert(position), Utility::Convert(scale), rotation, Utility::Convert(color),
										(thomas::resource::Font*)font->m_nativePtr);
	}

	void Camera::SetText(String^ id, String^ newText)
	{
		camera->GetGUIHandle()->SetText(Utility::ConvertString(id), Utility::ConvertString(newText));
	}

	void Camera::SetTextPosition(String^ id, Vector2 position)
	{
		camera->GetGUIHandle()->SetTextPosition(Utility::ConvertString(id), Utility::Convert(position));
	}

	void Camera::SetTextColor(String^ id, Vector4 color)
	{
		camera->GetGUIHandle()->SetTextColor(Utility::ConvertString(id), Utility::Convert(color));
	}

	void Camera::SetTextScale(String^ id, Vector2 scale)
	{
		camera->GetGUIHandle()->SetTextScale(Utility::ConvertString(id), Utility::Convert(scale));
	}

	void Camera::SetTextRotation(String^ id, float rotation)
	{
		camera->GetGUIHandle()->SetTextRotation(Utility::ConvertString(id), rotation);
	}
	void Camera::SetTextFont(String^ id, Font^ font)
	{
		camera->GetGUIHandle()->SetFont(Utility::ConvertString(id), (thomas::resource::Font*)font->m_nativePtr);
	}

	void Camera::DeleteText(String^ id)
	{
		camera->GetGUIHandle()->DeleteText(Utility::ConvertString(id));
	}

	//-------------------------------------------------------------------------------------------
}