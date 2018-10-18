#pragma once
#include <string>
#include <vector>
#include <windows.h>
namespace thomas
{
	class Scene;
	namespace object
	{
		class Object
		{
		private:
		public:
			
			virtual ~Object();
			void Destroy();
			std::string GetName();
			void SetName(std::string name);

			_GUID m_guid;
			virtual bool isDestroyed() { return m_destroyed; }
		protected:
			virtual void OnDestroy() {}
			
		private:
			bool m_destroyed = false;
		protected:
			std::string m_name;
		};
	}
}