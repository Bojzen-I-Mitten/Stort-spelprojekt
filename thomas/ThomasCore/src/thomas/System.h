#pragma once

#include "graphics/Renderer.h"

namespace thomas {
	namespace graphics
	{
		class Renderer;
	}

	class System {
	public:
		static graphics::Renderer S_RENDERER;
	};
}