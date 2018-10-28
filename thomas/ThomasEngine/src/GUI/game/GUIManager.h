#pragma once
#pragma unmanaged
#include <thomas/graphics/GUI/GUIManager.h>
#include <thomas/graphics/GUI/GUIElements.h>
#pragma managed
#include "../../object/component/Camera.h"

namespace ThomasEngine
{
	ref class GUIElement;
	public ref class GUIManager
	{
	private:
		static property List<GUIElement^> GUIElements
		{
			List<GUIElement^> get();
		}

		static thomas::graphics::GUI::GUIManager* nativePtr;

	public:
		static void AddImage(Texture2D^ texture);
		static void AddText(String^ text);
		static void DeleteGUIElement(GUIElement^ element);
	};
}