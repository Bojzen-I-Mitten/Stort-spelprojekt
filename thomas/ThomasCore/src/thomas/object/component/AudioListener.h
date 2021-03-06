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
				AudioListener() = default;
				~AudioListener() = default;

				void OnEnable();
				void OnDisable();

			public:
				void SetMasterVolume(float volume);	
				void SetMusicVolume(float volume);
				void SetFXVolume(float volume);
				void SetVoiceVolume(float volume);

			public:
				float GetMasterVolume() const;
				float GetMusicVolume() const;
				float GetFXVolume() const;
				float GetVoiceVolume() const;

				static AudioListener* GetInstance();

			private:
				static AudioListener* s_instance;
				static float s_masterVolume;
				static float s_musicVolume;
				static float s_fxVolume;
				static float s_voiceVolume;
			};
		}
	}

}