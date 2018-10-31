#include "AudioListener.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			AudioListener::AudioListener()
			{
			}

			AudioListener* AudioListener::GetInstance()
			{ 
				static AudioListener instance;

				return &instance;
			}
		}
	}
}