#include "Model.h"
#include "../object/GameObject.h"
#include "../object/component/Transform.h"
#include "Resources.h"
void ThomasEngine::Model::InitPrimitives()
{
	s_primitives = gcnew System::Collections::Generic::Dictionary<PrimitiveType, Model^>();
	LoadPrimitive(PrimitiveType::Cube, "../Data/primitives/cube.obj");
	LoadPrimitive(PrimitiveType::Plane, "../Data/primitives/plane.obj");
	LoadPrimitive(PrimitiveType::Cylinder, "../Data/primitives/cylinder.obj");
	LoadPrimitive(PrimitiveType::Quad, "../Data/primitives/quad.obj");
	LoadPrimitive(PrimitiveType::Sphere, "../Data/primitives/sphere.obj");
	LoadPrimitive(PrimitiveType::Capsule, "../Data/primitives/capsule.obj");
}
ThomasEngine::Model ^ ThomasEngine::Model::LoadPrimitive(PrimitiveType type, String ^ path)
{
	Model^ primitive = (Model^)Resources::Load(path);
	thomas::utils::Primitives::SetPrimitive((thomas::utils::Primitives::Type)type, (thomas::resource::Model*)primitive->m_nativePtr);
	s_primitives->Add(type, primitive);
	return primitive;
}


ThomasEngine::Model ^ ThomasEngine::Model::GetPrimitive(PrimitiveType type)
{
	return s_primitives[type];
}


void ThomasEngine::Model::GenerateBones(GameObject^ parent)
{
	thomas::resource::Model* nativeModel = ((thomas::resource::Model*)m_nativePtr);
	auto boneInfos = nativeModel->GetBones();

	System::Collections::Generic::Dictionary<unsigned int, Transform^> bones;

	for (int i = 0; i < boneInfos.size(); i++)
	{
		auto boneInfo = boneInfos[i];
		GameObject^ gObj = gcnew GameObject(Utility::ConvertString(boneInfo.name));
		bones[i] = gObj->transform;
		((thomas::object::component::Transform*)gObj->transform->nativePtr)->SetLocalMatrix(boneInfo.offsetMatrix);
	}

	for (int i = 0; i < boneInfos.size(); i++)
	{
		auto boneInfo = boneInfos[i];
		if (boneInfo.parentBone != i)
		{
			if (bones[i]->parent == nullptr)
			{
				Transform^ parentBone = bones[boneInfo.parentBone];
				bones[i]->parent = parentBone;
			}
			else
			{
				int x = 5;
			}
			
		}
	}
	
}
