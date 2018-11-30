#pragma once
#include <string>
namespace thomas
{
	namespace resource
	{
		
		class Resource
		{
		public:
			Resource(std::string path);
			virtual ~Resource() {};
			std::string GetName();
			std::string GetPath();
			const std::string& getName();
			uint32_t getHash();

			void Rename(std::string newPath);
			void Reload();
			static std::string PathToName(std::string path);

		protected:
			std::string m_path;

			void setName(std::string name);

		private:
			std::string m_name;
			uint32_t m_hash;
			virtual void OnChanged() = 0;
		};
	}
}