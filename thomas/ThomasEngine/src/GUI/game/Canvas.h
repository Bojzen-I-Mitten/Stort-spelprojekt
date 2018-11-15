#pragma once
#pragma unmanaged
#pragma managed
namespace thomas { namespace graphics { namespace GUI { class Canvas; } } }
namespace ThomasEngine
{
	ref class Camera;
	ref class Texture2D;
	ref class GUIElement;
	ref class Image;
	ref class Text;
	value class Viewport;
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

		property bool isRendering
		{
			void set(bool rendering);
			bool get();
		}

	private:
		thomas::graphics::GUI::Canvas* nativePtr;
	public:
		Image^ Add(Texture2D^ texture);
		Text^ Add(System::String^ text);
		void Remove(GUIElement^ element);
	};
}