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
