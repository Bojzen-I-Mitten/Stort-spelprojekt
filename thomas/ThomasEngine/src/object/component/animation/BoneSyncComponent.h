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
		[ExecuteInEditor]
		public ref class BoneSyncComponent : public Component
		{
		public:
			enum class BoneSyncMode
			{
				Local,
				Global
			};
			
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

			property BoneSyncMode Mode
			{
				BoneSyncMode get();
				void set(BoneSyncMode value);
			}

			property Matrix Offset
			{
				Matrix get();
				void set(Matrix value);
			}
		internal:

			virtual void OnParentDestroy(GameObject^ relative) override;
			virtual void Awake() override;
			virtual void Update() override;

		private:
			GameObject^ m_skeletonSrc;
			System::String^ m_boneName;
			uint32_t m_boneHash, m_boneIndex;
			BoneSyncMode m_mode;
			Matrix m_offset;
			RenderSkinnedComponent^ m_updateSrc;

			void refreshBoneIndex();
		};
}