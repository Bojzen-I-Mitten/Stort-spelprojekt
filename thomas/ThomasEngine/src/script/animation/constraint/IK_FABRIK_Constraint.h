#pragma once
#pragma unmanaged
#include <thomas/graphics/animation/constraint/IK_FABRIK_Constraint.h>
#include <thomas/object/component/RenderSkinnedComponent.h>
#pragma managed

namespace ThomasEngine
{
	ref class GameObject;
	ref class RenderSkinnedComponent;
	ref class Model;

	namespace Script
	{
		public ref class IK_FABRIK_Constraint
		{
		public:
			IK_FABRIK_Constraint(uint32_t num_link);
			~IK_FABRIK_Constraint();

			property Vector3 Target {
				Vector3 get();
				void set(Vector3 mode);
			}
			/* Targeted orientation
			*/
			property Quaternion Orienation {
				Quaternion get();
				void set(Quaternion rot);
			}
			/* Weight applied
			*/
			property float Weight {
				float get();
				void set(float w);
			}

			void apply(GameObject^ obj, uint32_t boneIndex);
			void apply(RenderSkinnedComponent^ skinn, uint32_t boneIndex);
			void apply(thomas::object::component::RenderSkinnedComponent* skinn, uint32_t boneIndex);

			thomas::graphics::animation::IK_FABRIK_Constraint* Native();
		private:
			uint32_t m_num_link;
			thomas::graphics::animation::IK_FABRIK_Constraint* m_ptr;
		};
	}
}