#pragma once
#include "Component.h"
namespace DirectX { class AudioListener; }
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
				void Update();
				static AudioListener* GetInstance();
				DirectX::AudioListener& GetListner();
			private:
				static AudioListener* s_instance;
				DirectX::AudioListener* m_listner;
			};
		}
	}

}