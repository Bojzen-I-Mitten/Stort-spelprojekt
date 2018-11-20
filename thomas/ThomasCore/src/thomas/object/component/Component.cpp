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
			}
			void Component::setEnabledState(bool state)
			{
				m_enabled = state;
			}
		}
	}
}
