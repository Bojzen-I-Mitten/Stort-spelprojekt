#pragma once
#pragma unmanaged
#include <thomas/graphics/animation/constraint/IK_FABRIK_Constraint.h>
#include <thomas/object/component/RenderSkinnedComponent.h>
#pragma managed

namespace ThomasEngine
{
	ref class GameObject;
	ref class RenderSkinnedComponent;

	namespace Script
	{
		public ref class IK_FABRIK_Constraint
		{
		public:
			IK_FABRIK_Constraint();
			~IK_FABRIK_Constraint();

			property float Weight {
				float get();
				void set(float w);
			}
			property Vector3 Target {
				Vector3 get();
				void set(Vector3 mode);
			}

			void apply(GameObject^ obj, uint32_t boneIndex);
			void apply(RenderSkinnedComponent^ skinn, uint32_t boneIndex);
			void apply(thomas::object::component::RenderSkinnedComponent* skinn, uint32_t boneIndex);

			thomas::graphics::animation::IK_FABRIK_Constraint* Native();
		private:

			thomas::graphics::animation::IK_FABRIK_Constraint* m_ptr;
		};
	}
}