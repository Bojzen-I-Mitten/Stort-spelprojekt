#pragma unmanaged
#include <thomas/object/component/Camera.h>

#pragma managed
#include "Camera.h"
#include "../../Utility.h"
#include "../../Debug.h"
#include "../../resource/texture/Texture2D.h"
#include "../../resource/Font.h"
#include "../../GUI/game/GUIElements.h"
#include "../../math/Viewport.h"

namespace ThomasEngine
{
	Camera::Camera() : Component(new thomas::object::component::Camera()) { renderGUI = true; }

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

	Viewport Camera::viewport::get()
	{
		return Utility::Convert(((thomas::object::component::Camera*)nativePtr)->GetViewport());
	}

	Canvas^ Camera::AddCanvas() { return gcnew Canvas(this); }
	Canvas^ Camera::AddCanvas(Viewport viewport) { return gcnew Canvas(this, viewport); }
}