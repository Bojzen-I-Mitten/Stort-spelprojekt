#pragma unmanaged
#include <thomas\resource\Model.h>
#include <thomas\utils\Primitives.h>
#include <thomas\graphics\Mesh.h>
#include <thomas/graphics/animation/data/Skeleton.h>
#pragma managed
#include "Model.h"
#include "../object/GameObject.h"
#include "Resources.h"
namespace ThomasEngine
{
	Model::Model(thomas::resource::Model* nativePtr)
		: Resource(Utility::ConvertString(nativePtr->GetPath()), nativePtr)
	{}
	Model::Model(String^ path) :
		Resource(path, new thomas::resource::Model(Utility::ConvertString(path)))
	{};

	Model::~Model()
	{
		if (m_nativePtr)
			delete m_nativePtr;
	}
	void Model::OnDeserialized(StreamingContext c)
	{
		m_nativePtr = new thomas::resource::Model(Utility::ConvertString(Path));
	}

	thomas::resource::Model * Model::Native()
	{
		return (thomas::resource::Model*)m_nativePtr;
	}

	Model^ Model::GetPrimitive(PrimitiveType type)
	{ 
		return gcnew Model(thomas::utils::Primitives::GetPrimitive((thomas::utils::Primitives::Type)type)); 
	}



	void Model::InitPrimitives()
	{
		LoadPrimitive(PrimitiveType::Cube, "../Data/primitives/box.obj");
		LoadPrimitive(PrimitiveType::Plane, "../Data/primitives/plane.obj");
		LoadPrimitive(PrimitiveType::Cylinder, "../Data/primitives/cylinder.obj");
		LoadPrimitive(PrimitiveType::Quad, "../Data/primitives/quad.obj");
		LoadPrimitive(PrimitiveType::Sphere, "../Data/primitives/sphere.obj");
		LoadPrimitive(PrimitiveType::Capsule, "../Data/primitives/capsule.obj");
		LoadPrimitive(PrimitiveType::Monkey, "../Data/primitives/monkey.obj");
		LoadPrimitive(PrimitiveType::Torus, "../Data/primitives/torus.obj");
	}
	Model ^ Model::LoadPrimitive(PrimitiveType type, String ^ path)
	{
		Model^ primitive = (Model^)Resources::LoadSysPath(path);
		thomas::utils::Primitives::SetPrimitive((thomas::utils::Primitives::Type)type, (thomas::resource::Model*)primitive->m_nativePtr);
		return primitive;
	}
	void Model::GenerateBones(GameObject^ parent)
	{
		/*
		LOG("Model::GenerateBones deprecated/TODO");
		thomas::resource::Model* nativeModel = ((thomas::resource::Model*)m_nativePtr);
		auto skel = nativeModel->GetSkeleton();


		thomas::resource::Model* nativeModel = ((thomas::resource::Model*)m_nativePtr);
		auto boneInfos = nativeModel->GetBones();

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
	}