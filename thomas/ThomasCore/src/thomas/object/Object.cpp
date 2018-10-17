#include "Object.h"


namespace thomas
{
	namespace object
	{
		Object::~Object()
		{
		}

		
		std::string Object::GetName()
		{
			return m_name;
		}
		void Object::SetName(std::string name)
		{
			m_name = name;

		}
		void Object::Destroy()
		{
			if (!m_destroyed)
			{
				OnDestroy();
				m_destroyed = true;
			}
		}

	}
}

