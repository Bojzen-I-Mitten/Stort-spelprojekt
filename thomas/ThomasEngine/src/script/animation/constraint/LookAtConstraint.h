#pragma once

#pragma unmanaged
#include <thomas/graphics/animation/constraint/LookAtConstraint.h>
#include <thomas/object/component/RenderSkinnedComponent.h>
#pragma managed

namespace ThomasEngine
{
	ref class GameObject;
	ref class RenderSkinnedComponent;

	namespace Script
	{
		public ref class LookAtConstraint 
		{
		public:
			enum class AxisConstraint {
				AxisX,
				AxisY,
				AxisZ,
				AxisXYZ
			};
		public:
			LookAtConstraint(AxisConstraint mode);
			~LookAtConstraint();

			property AxisConstraint Mode {
				AxisConstraint get();
				void set(AxisConstraint mode);
			}
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

			thomas::graphics::animation::LookAtConstraint* Native();
		private:

			thomas::graphics::animation::LookAtConstraint* m_ptr;
		};
	}
}