#pragma once
#include "../../utils/Math.h"

namespace thomas
{
	namespace graphics
	{
		namespace render 
		{
			/* Camera frame data
			*/
			struct CAMERA_FRAME_DATA
			{
				int targetDisplay;
				math::Viewport viewport;
				math::Matrix projectionMatrix;
				math::Matrix viewMatrix;
				math::Vector4 position;
			};
		}
	}
}