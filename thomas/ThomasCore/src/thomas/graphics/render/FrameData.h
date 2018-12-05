#pragma once
#include "../../utils/Math.h"

namespace thomas
{
	namespace resource
	{
		class RenderTexture;
	}
	namespace graphics
	{
		namespace render 
		{
			/* Camera frame data
			*/
			struct CAMERA_FRAME_DATA
			{
				int targetDisplay;
				resource::RenderTexture* renderTexture = nullptr;
				math::Viewport viewport;
				math::Matrix projectionMatrix;
				math::Matrix viewMatrix;
				math::Vector4 position;
			};
		}
	}
}