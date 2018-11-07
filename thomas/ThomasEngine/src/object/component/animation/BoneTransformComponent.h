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
			[Newtonsoft::Json::JsonPropertyAttribute(IsReference = true)]
			property GameObject^ AnimatedObject {
				GameObject^ get();
				void set(GameObject^ value);
			}

			property System::String^ BoneName {
				System::String^ get();
				void set(System::String^ value);
			}

			void OnParentDestroy(GameObject^ relative) override;
			void OnAwake() override;

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