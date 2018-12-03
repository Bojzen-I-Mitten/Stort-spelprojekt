#include "Resource.h"
#include <Windows.h>
#include "Shlwapi.h"

#include "../utils/Utility.h"
namespace thomas
{
	namespace resource
	{
		Resource::Resource(std::string path)
		{
			m_path = path;

			m_name = PathFindFileName(m_path.c_str());
			//m_name = m_name.substr(0, m_name.find_last_of('.'));
			// Call full function
			setName(m_name.substr(0, m_name.find_last_of('.')));
		}

		std::string Resource::GetName()
		{
			return m_name;
		}

		std::string Resource::GetPath()
		{
			return m_path;
		}

		uint32_t Resource::getHash()
		{
			return m_hash;
		}

		void Resource::Rename(std::string newPath)
		{
			m_path = newPath;

			m_name = PathFindFileName(m_path.c_str());
			m_name = m_name.substr(0, m_name.find_last_of('.'));

			OnChanged();
		}

		void Resource::Reload()
		{
			OnChanged();
		}

		std::string Resource::PathToName(std::string path)
		{
			std::string name = PathFindFileName(path.c_str());
			name = name.substr(0, name.find_last_of('.'));
			return name;
		}
		void Resource::setName(const std::string& name)
		{
			m_name = name;
			m_hash = utility::hash(name);
		}
		const std::string & Resource::getName()
		{
			return m_name;
		}
	}
}
