#pragma once
#pragma unmanaged
#pragma managed
namespace thomas { namespace graphics { namespace GUI { class Canvas; } } }
namespace ThomasEngine
{
	ref class Camera;
	ref class Viewport;
	ref class Texture2D;
	ref class GUIElement;
	public ref class Canvas
	{
	public:
		Canvas(Camera^ camera);
		Canvas(Camera^ camera, Viewport viewport);

		Camera^ camera;

		property Viewport viewport
		{
			Viewport get();
			void set(Viewport viewport);
		}

	private:
		thomas::graphics::GUI::Canvas* nativePtr;
	public:
		GUIElement^ AddGUIElement(Texture2D^ texture);
		GUIElement^ AddGUIElement(System::String^ text);
		void DeleteGUIElement(GUIElement^ element);
	};
}