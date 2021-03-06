#pragma once

#include "Texture2D.h"
#include "../../utils/Math.h"
#include "../../utils/d3d.h"
#include <DirectXTex.h>

namespace thomas
{
	namespace graphics
	{
		namespace GUI
		{
			class Canvas;
		}
	}
	namespace resource
	{
		class RenderTexture : public Texture2D
		{
		public:
			RenderTexture(int width, int height);
			virtual ~RenderTexture();
			void WriteCanvas(graphics::GUI::Canvas* canvas);
			void Bind();
		private:
			ID3D11CommandList* m_commandList;
			ID3D11RenderTargetView* m_rtv;
		};
	}
}