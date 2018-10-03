#pragma once

#pragma unmanaged
#include<thomas/graphics/animation/constraint/LookAtConstraint.h>
#pragma managed

namespace ThomasEngine
{
	ref class GameObject;

	namespace Script
	{
		public ref class LookAtConstraint 
		{
		private:
		public:
			LookAtConstraint();
			~LookAtConstraint();


			void apply(GameObject^ obj, uint32_t boneIndex);

			thomas::graphics::animation::LookAtConstraint* Native();
		private:

			thomas::graphics::animation::LookAtConstraint* m_ptr;
		};
	}
}