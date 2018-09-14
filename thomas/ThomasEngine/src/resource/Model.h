#pragma once
#pragma unmanaged
#include <thomas\resource\Model.h>
#include <thomas\utils\Primitives.h>
#include <thomas\graphics\Mesh.h>
#include <memory>
#pragma managed
#include "Resource.h"

namespace ThomasEngine
{
	public enum class PrimitiveType //could not typedef because no ToString
	{
		Sphere,
		Capsule,
		Cylinder,
		Cube,
		Plane,
		Quad
	};

	[DataContractAttribute]
	public ref class Model : public Resource
	{
	internal:
		static System::Collections::Generic::Dictionary<PrimitiveType, Model^>^ s_primitives;
		Model(thomas::resource::Model* nativePtr);
	public:
		Model(String^ path);

		void GenerateBones(GameObject^ parent);
		~Model();
		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);
						
		static Model^ GetPrimitive(PrimitiveType type);
		static void InitPrimitives();
		Model ^ LoadPrimitive(PrimitiveType type, String ^ path);
	};
}
