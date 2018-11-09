#include "AudioListener.h"
#include "../../Common.h"
#include "../../ThomasCore.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			AudioListener* AudioListener::s_instance = nullptr;
			AudioListener::AudioListener()
			{
			}

			void AudioListener::OnEnable()
			{
				if (s_instance)
					LOG("Cannot have multiple AudioListeners in a scene");
				s_instance = this;
			}

			void AudioListener::OnDisable()
			{
				s_instance = nullptr;
			}

			AudioListener * AudioListener::GetInstance()
			{
				return s_instance;
			}
		}
	}
}

