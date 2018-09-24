#pragma unmanaged
#include <thomas\object\component\Camera.h>
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


}