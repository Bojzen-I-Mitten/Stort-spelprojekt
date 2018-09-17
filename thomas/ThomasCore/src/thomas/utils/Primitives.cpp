#include "Primitives.h"
#include "../resource/Model.h"
namespace thomas
{
	namespace utils
	{
		std::map<Primitives::Type, resource::Model*> Primitives::s_primitives;


		void Primitives::SetPrimitive(Type type, resource::Model * model)
		{
			s_primitives[type] = model;
		}

		void Primitives::Destroy()
		{
			s_primitives.clear();
		}

		resource::Model* Primitives::GetPrimitive(Type type)
		{
			return s_primitives[type];
		}

	}
}
