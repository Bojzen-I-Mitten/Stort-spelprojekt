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
			virtual void OnDestroy() {}
			std::string GetName();
			void SetName(std::string name);

			static bool Destroy(Object *object);
			static void Destroy();

			static std::vector<Object*> GetObjects();
			static void Add(Object* object);

			template<typename T>
			static std::vector<T*> FindObjectsOfType();

			static void Clean();

		
		private:
			bool m_destroyed = false;
			static std::vector<Object*> s_objects;

		protected:
			std::string m_name;

		public:
			_GUID m_guid;

		};



		template<typename T>
		inline std::vector<T*> Object::FindObjectsOfType()
		{
			std::vector<T*> objects;
			for (UINT i = 0; i < s_objects.size(); i++)
			{
				T* object = dynamic_cast<T*>(s_objects[i]);
				if (object)
					objects.push_back(object);
			}
			return objects;
		}


	}
}