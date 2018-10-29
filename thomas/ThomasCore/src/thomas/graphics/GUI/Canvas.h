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

using namespace DirectX;

namespace thomas
{
	using namespace resource;
	using namespace math;
	namespace graphics
	{
		namespace GUI
		{
			struct GUIElement;
			class Canvas
			{
			public:
				//Canvas();
				Canvas(Viewport viewport, Viewport* camViewport, Vector2 baseResolution = Vector2(1920, 1080));
				~Canvas() = default;
				void Destroy();
				void Render();

			public:
				Viewport GetViewport();
				void SetViewport(Viewport viewport);
				void UpdateViewportScale();
				GUIElement* AddGUIElement(const std::string & text);
				GUIElement* AddGUIElement(Texture2D* texture);
				void DeleteGUIElement(GUIElement* element);

			private:
				std::vector<std::unique_ptr<GUIElement>> m_GUIElements;
				std::unique_ptr<CommonStates> m_spriteStates;
				std::unique_ptr<SpriteBatch> m_spriteBatch;
				std::unique_ptr<Font> m_defaultFont;
				math::Vector2 m_viewportScale;
				Viewport m_viewport;
				Viewport* m_camViewport;
				math::Vector2 m_baseResolution;
			};
		}
	}
}