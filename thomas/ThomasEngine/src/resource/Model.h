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
		Quad
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

		property thomas::resource::Model* Native
		{
			thomas::resource::Model* get() { return reinterpret_cast<thomas::resource::Model*>(m_nativePtr); }
		}
						
		static Model^ GetPrimitive(PrimitiveType type);
		static void InitPrimitives();
		static Model ^ LoadPrimitive(PrimitiveType type, String ^ path);
	};
}
