#pragma once
#include "Resource.h"

namespace thomas { namespace resource { class Model; } }
namespace ThomasEngine
{
	ref class GameObject;
	public enum class PrimitiveType //could not typedef because no ToString
	{
		Sphere,
		Capsule,
		Cylinder,
		Cube,
		Plane,
		Quad,
		Monkey,
		Torus
	};

	[DataContractAttribute]
	public ref class Model : public Resource
	{
	internal:
		Model(thomas::resource::Model* nativePtr);
	public:
		Model(String^ path);

		void GenerateBones(GameObject^ parent);
		~Model();
		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);

		thomas::resource::Model* Native();
						
		static Model^ GetPrimitive(PrimitiveType type);
		static void InitPrimitives();
		static Model ^ LoadPrimitive(PrimitiveType type, String ^ path);
	};
}
