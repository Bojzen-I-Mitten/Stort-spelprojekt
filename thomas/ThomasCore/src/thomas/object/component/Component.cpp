#include "Component.h"

#include "../GameObject.h"

namespace thomas
{
	namespace object
	{
		namespace component
		{
			Component::Component() :
				m_enabled(false), m_lock()
#ifdef _EDITOR
				, m_editorLock()
#endif
			{
			}


			thomas::object::component::Component::~Component()
			{
				for (int i = 0; i < m_gameObject->m_components.size(); i++)
				{
					Component* component = m_gameObject->m_components[i];
					if (component == this)
					{
						m_gameObject->m_components.erase(m_gameObject->m_components.begin() + i);
					}
				}
			}
			void Component::setEnabledState(bool state)
			{
				m_enabled = state;
			}
		}
	}
}
