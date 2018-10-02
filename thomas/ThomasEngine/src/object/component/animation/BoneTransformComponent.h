#pragma once
#include "../../Component.h"

#pragma unmanaged

#include <thomas/object/component/BoneTransformComponent.h>

#pragma managed


namespace thomas {
	namespace object {
		namespace component {
			class BoneTransformComponent;
		}
	}
}


namespace ThomasEngine
{

	ref class GameObject;

		[DisallowMultipleComponent]
		public ref class BoneTransformComponent : public Component
		{
		public:
			BoneTransformComponent();
			property GameObject^ AnimatedObject {
				GameObject^ get();
				void set(GameObject^ value);
			}

			property System::String^ BoneName {
				System::String^ get();
				void set(System::String^ value);
			}
		private:
			property thomas::object::component::BoneTransformComponent* Native
			{
				thomas::object::component::BoneTransformComponent* get();
			}
		private:
			GameObject^ m_skeletonSrc;
			System::String^ m_boneName;
		};
}