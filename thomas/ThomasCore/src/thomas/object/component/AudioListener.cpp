#include "AudioListener.h"
#include "../../Common.h"
#include "../../ThomasCore.h"
#include "Transform.h"
#include "../GameObject.h"
#include "../../ThomasTime.h"
#include "../../Sound.h"
namespace thomas
{
	namespace object
	{
		namespace component
		{
			AudioListener* AudioListener::s_instance = nullptr;
			AudioListener::AudioListener()
			{
				m_listner = new DirectX::AudioListener();
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

			void AudioListener::Update()
			{
				m_listner->Update(m_gameObject->m_transform->GetPosition(), m_gameObject->m_transform->Up(), ThomasTime::GetActualDeltaTime());
			}

			AudioListener * AudioListener::GetInstance()
			{
				return s_instance;
			}
			DirectX::AudioListener & AudioListener::GetListner()
			{
				return *m_listner;
			}
		}
	}
}

