#pragma once

#pragma unmanaged
#include <thomas/graphics/animation/constraint/PositionConstraint.h>
#include <thomas/object/component/RenderSkinnedComponent.h>
#pragma managed

namespace ThomasEngine
{
	ref class GameObject;
	ref class RenderSkinnedComponent;

	namespace Script
	{
		public ref class PositionConstraint
		{
		public:
			PositionConstraint();
			~PositionConstraint();

			property Vector3 Position {
				Vector3 get();
				void set(Vector3 mode);
			}
			property float Distance {
				float get();
				void set(float Distance);
			}
			property bool Active {
				bool get();
				void set(bool state);
			}

			void apply(GameObject^ obj, uint32_t boneIndex);
			void apply(RenderSkinnedComponent^ skinn, uint32_t boneIndex);
			void apply(thomas::object::component::RenderSkinnedComponent* skinn, uint32_t boneIndex);
			/* Remove the constraint
			*/
			void disable();

			thomas::graphics::animation::PositionConstraint* Native();
		private:

			thomas::graphics::animation::PositionConstraint* m_ptr;			// Pointer to the constraint
			thomas::object::component::RenderSkinnedComponent * m_skinn;	// Skinned component constraint added to
			uint32_t m_boneIndex;											// Index to the bone constraint is added to
		};
	}
}