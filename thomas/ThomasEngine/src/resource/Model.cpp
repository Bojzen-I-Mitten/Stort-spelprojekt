#include "Model.h"
#include "../object/GameObject.h"
#include <thomas/graphics/animation/data/Skeleton.h>
void ThomasEngine::Model::GenerateBones(GameObject^ parent)
{
	LOG("Model::GenerateBones deprecated/TODO");
	/*
	thomas::resource::Model* nativeModel = ((thomas::resource::Model*)m_nativePtr);
	auto skel = nativeModel->GetSkeleton();

	System::Collections::Generic::Dictionary<unsigned int, Transform^> boneInfo;

	for (int i = 0; i < skel->getNumBones(); i++)
	{
		auto boneInfo = skel->getBone(i);
		GameObject^ gObj = gcnew GameObject(Utility::ConvertString(boneInfo._boneName));
		this->boneInfo[i] = gObj->transform;
		((thomas::object::component::Transform*)gObj->transform->nativePtr)->SetLocalMatrix(boneInfo._bindPose);
	}

	for (int i = 0; i < skel->getNumBones(); i++)
	{
		auto boneInfo = skel->getBone(i);
		if (boneInfo._parentIndex != i)
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
		*/
	
}
