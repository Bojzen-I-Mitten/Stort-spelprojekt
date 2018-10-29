#pragma unmanaged
#include <thomas/graphics/GUI/Canvas.h>
#include <thomas/object/component/Camera.h>
#pragma managed
#include "../../Utility.h"
#include "Canvas.h"
#include "GUIElements.h"
#include "../../object/component/Camera.h"
#include "../../math/Viewport.h"
#include "../../resource/texture/Texture2D.h"

namespace ThomasEngine
{
	Canvas::Canvas(Camera^ camera)
	{
		this->camera = camera;
		viewport = camera->viewport;

		nativePtr = ((thomas::object::component::Camera*)camera->nativePtr)->AddCanvas();
	}

	Canvas::Canvas(Camera^ camera, Viewport viewport)
	{
		this->camera = camera;
		this->viewport = viewport;

		nativePtr = ((thomas::object::component::Camera*)camera->nativePtr)->AddCanvas(Utility::Convert(viewport));
	}

	Camera^ Canvas::camera::get() { return camera; }
	void Canvas::camera::set(Camera^ camera) { this->camera = camera; }

	Viewport Canvas::viewport::get()
	{ 
		return Utility::Convert(nativePtr->GetViewport());
	}
	void Canvas::viewport::set(Viewport viewport) { nativePtr->SetViewport(Utility::Convert(viewport)); }

	GUIElement^ Canvas::AddGUIElement(Texture2D ^ texture)
	{
		thomas::graphics::GUI::GUIElement* image =
			nativePtr->AddGUIElement((thomas::resource::Texture2D*)texture->m_nativePtr);

		return %Image(image);
	}

	GUIElement^ Canvas::AddGUIElement(String ^ text)
	{
		thomas::graphics::GUI::GUIElement* newText =
			nativePtr->AddGUIElement(Utility::ConvertString(text));

		return %Text(newText);
	}

	void Canvas::DeleteGUIElement(GUIElement ^ element)
	{
		nativePtr->DeleteGUIElement(element->nativePtr);
	}
}