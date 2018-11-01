#pragma once
#include "../../Component.h"

#pragma unmanaged

#include "../RenderSkinnedComponent.h"

#pragma managed




namespace ThomasEngine
{

	ref class GameObject;
	ref class RenderSkinnedComponent;

		[DisallowMultipleComponent]
		public ref class BoneSyncComponent : public Component
		{
		public:
			BoneSyncComponent();
			[Newtonsoft::Json::JsonPropertyAttribute(IsReference = true)]
			property GameObject^ AnimatedObject {
				GameObject^ get();
				void set(GameObject^ value);
			}

			property System::String^ BoneName {
				System::String^ get();
				void set(System::String^ value);
			}

			[BrowsableAttribute(false)]
			property Matrix Offset
			{
				Matrix get();
				void set(Matrix value);
			}

			void OnParentDestroy(GameObject^ relative) override;
			void Awake() override;
			void Update() override;

		private:
			GameObject^ m_skeletonSrc;
			System::String^ m_boneName;
			uint32_t m_boneHash, m_boneIndex;
			Matrix m_offset;
			RenderSkinnedComponent^ m_updateSrc;

			void refreshBoneIndex();
		};
}