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
			/* Pause animation. Stop timer from being updated.	*/
			void Pause();
			void Loop();
			/*	Continue. Playing from paused state. */
			void Continue();
			void SetFrame(float elapsed);
			void SetSpeed(float speed);
			float GetSpeed();

			bool isPlaying();

			thomas::graphics::animation::Playback * Native();

		private:

			thomas::graphics::animation::PlayType m_type;
			thomas::graphics::animation::BaseAnimationTime * m_handle;
		};

	}
}