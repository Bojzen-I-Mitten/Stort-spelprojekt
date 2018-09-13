#include "Model.h"
#include "../object/GameObject.h"
void ThomasEngine::Model::GenerateBones(GameObject^ parent)
{
	thomas::resource::Model* nativeModel = ((thomas::resource::Model*)m_nativePtr);
	auto boneInfos = nativeModel->GetBones();

	System::Collections::Generic::Dictionary<unsigned int, Transform^> boneInfo;

	for (int i = 0; i < boneInfos.size(); i++)
	{
		auto boneInfo = boneInfos[i];
		GameObject^ gObj = gcnew GameObject(Utility::ConvertString(boneInfo.name));
		boneInfo[i] = gObj->transform;
		((thomas::object::component::Transform*)gObj->transform->nativePtr)->SetLocalMatrix(boneInfo.offsetMatrix);
	}

	for (int i = 0; i < boneInfos.size(); i++)
	{
		auto boneInfo = boneInfos[i];
		if (boneInfo.parentBone != i)
		{
			if (boneInfo[i]->parent == nullptr)
			{
				Transform^ parentBone = boneInfo[boneInfo.parentBone];
				boneInfo[i]->parent = parentBone;
			}
			else
			{
				int x = 5;
			}
			
		}
	}
	
}
