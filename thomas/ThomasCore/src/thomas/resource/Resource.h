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
			const std::string& getName();
			/* Set the name hash.
			*/
			void setName(const std::string& name);
			std::string GetPath();
			/* Set path, don't ever do this... 
			*/
			void SetPath(std::string path);
			uint32_t getHash();

			void Rename(std::string newPath);
			void Reload();
			static std::string PathToName(std::string path);

		protected:
			std::string m_path;


		private:
			std::string m_name;
			uint32_t m_hash;
			virtual void OnChanged() = 0;
		};
	}
}