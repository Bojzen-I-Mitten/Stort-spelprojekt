#pragma once

// C++
#include <map>

// DirectXTK
#include <DirectXTK/SpriteBatch.h>
#include <DirectXTK/CommonStates.h>

// Thomas
#include "../../resource/texture/Texture2D.h"
#include "../../utils/Math.h"
#include "../../resource/Font.h"
#include "../../object/Object.h"
#include "../../utils/atomic/Synchronization.h"

using namespace DirectX;

namespace thomas
{
	namespace object { namespace component { class Camera; } }
	using namespace resource;
	using namespace math;
	namespace graphics
	{
		namespace GUI
		{
			struct GUIElement;
			class Canvas 
				: public object::Object
			{
			public:
				//Canvas();
				Canvas(Viewport viewport, object::component::Camera* cam, Vector2 baseResolution = Vector2(1920, 1080));
				virtual ~Canvas();
				void Render();

			public:
				Viewport GetViewport(); //returns the actual viewport for the canvas
				Vector2 GetViewportScale();
				void SetViewport(Viewport viewport);
				GUIElement* Add(const std::string & text);
				GUIElement* Add(Texture2D* texture);
				void Remove(GUIElement* element);
				void SetRendering(bool render);
				bool GetRendering();
				void SetWorldMatrix(math::Matrix value);
				void Set3D(bool value);
				bool Get3D();
			private:
				
				std::vector<std::unique_ptr<GUIElement>> m_GUIElements;
				CommonStates* m_spriteStates;
				SpriteBatch* m_spriteBatch;
				Font* m_defaultFont;
				Viewport m_viewport;
				object::component::Camera* m_camera;
				math::Vector2 m_baseResolution;
				math::Matrix m_worldMatrix;
				bool m_render;
				utils::atomics::SpinLock m_lock;
			};
		}
	}
}