#include "PlaybackHandle.h"
#include <algorithm>

namespace ThomasEngine
{
	namespace Script
	{

		PlaybackHandle::PlaybackHandle(thomas::graphics::animation::BaseAnimationTime * handle)
			: m_handle(handle), m_type(thomas::graphics::animation::PlayType::None)
		{	}
		PlaybackHandle::~PlaybackHandle()
		{
		}
		void PlaybackHandle::Play()
		{
			m_handle->playOnce();
		}
		void PlaybackHandle::Continue()
		{
			if (m_type == thomas::graphics::animation::PlayType::None) return;
			m_handle->setPlayType(thomas::graphics::animation::PlayType::Once);
			m_type = thomas::graphics::animation::PlayType::None;
		}
		void PlaybackHandle::Pause()
		{
			if (m_type == thomas::graphics::animation::PlayType::None) return;
			m_type = m_handle->getPlayType();
			m_handle->setPlayType(thomas::graphics::animation::PlayType::None);
		}
		void PlaybackHandle::Loop()
		{
			m_handle->setPlayType(thomas::graphics::animation::PlayType::Loop);
		}
		void PlaybackHandle::SetFrame(float elapsed)
		{
			m_handle->m_elapsedTime = std::fmodf(elapsed, m_handle->getDuration());
		}
		thomas::graphics::animation::Playback * PlaybackHandle::Native()
		{
			return m_handle;
		}
	}
}