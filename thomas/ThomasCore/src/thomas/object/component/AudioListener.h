#pragma once
#include "Component.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			class AudioListener : public Component
			{
			public:
				AudioListener();
				~AudioListener() = default;

				void OnEnable();
				void OnDisable();

				static AudioListener* GetInstance();

			private:
				static AudioListener* s_instance;
			};
		}
	}

}