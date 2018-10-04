#pragma once

#pragma unmanaged

#include <thomas/graphics/animation/BaseAnimationTime.h>

#pragma managed

namespace ThomasEngine
{
	namespace Script
	{
		public ref class PlaybackHandle {
		public:
			/* Generate  */
			PlaybackHandle(thomas::graphics::animation::BaseAnimationTime * handle);
			~PlaybackHandle();

			void Play();
			void Pause();
			void Loop();
			void Continue();
			void SetFrame(float elapsed);

			thomas::graphics::animation::Playback * Native();

		private:

			thomas::graphics::animation::PlayType m_type;
			thomas::graphics::animation::BaseAnimationTime * m_handle;
		};

	}
}