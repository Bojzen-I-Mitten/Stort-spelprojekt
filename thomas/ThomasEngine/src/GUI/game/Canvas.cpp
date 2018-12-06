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
		nativePtr = ((thomas::object::component::Camera*)camera->nativePtr)->AddCanvas();
	}

	Canvas::Canvas(Camera^ camera, Viewport viewport)
	{
		this->camera = camera;
		nativePtr = ((thomas::object::component::Camera*)camera->nativePtr)->AddCanvas(Utility::Convert(viewport));
	}

	Canvas::Canvas(Viewport viewport)
	{
		this->camera = nullptr;
		nativePtr = new thomas::graphics::GUI::Canvas(Utility::Convert(viewport), nullptr, Utility::Convert(viewport.size));
	}

	Viewport Canvas::viewport::get()
	{ 
		return Utility::Convert(nativePtr->GetViewport());
	}
	void Canvas::viewport::set(Viewport viewport)
	{
		nativePtr->SetViewport(Utility::Convert(viewport)); 
	}

	void Canvas::isRendering::set(bool rendering)
	{
		nativePtr->SetRendering(rendering);
	}

	bool Canvas::isRendering::get()
	{
		return nativePtr->GetRendering();
	}

	void Canvas::worldMatrix::set(Matrix value)
	{
		nativePtr->SetWorldMatrix(Utility::Convert(value));
	}

	void Canvas::is3D::set(bool value)
	{
		nativePtr->Set3D(value);
	}

	bool Canvas::is3D::get()
	{
		return nativePtr->Get3D();
	}

	Image^ Canvas::Add(Texture2D ^ texture)
	{
		if(texture == nullptr)
		{
			return nullptr;
		}
		thomas::graphics::GUI::GUIElement* image =
			nativePtr->Add((thomas::resource::Texture2D*)texture->m_nativePtr);

		return %Image(image);
	}

	Text^ Canvas::Add(System::String ^ text)
	{
		thomas::graphics::GUI::GUIElement* newText =
			nativePtr->Add(Utility::ConvertString(text));

		return %Text(newText);
	}

	void Canvas::Remove(GUIElement ^ element)
	{
		nativePtr->Remove(element->nativePtr);
	}
}