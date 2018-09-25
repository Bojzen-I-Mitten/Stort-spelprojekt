#pragma once
#pragma unmanaged
#include <thomas\resource\Shader.h>
#pragma managed
#include "resource.h"
namespace ThomasEngine
{
	[DataContractAttribute]
	public ref class Shader : public Resource
	{
	internal:
		Shader(thomas::resource::Shader* ptr);
		Shader(String^ path);

	public:
		

		static Shader^ Find(String^ name);

		static void SetGlobalColor(String^ name, Color value);
		static void SetGlobalFloat(String^ name, float value);
		static void SetGlobalInt(String^ name, int value);
		static void SetGlobalMatrix(String^ name, Matrix value);
		static void SetGlobalVector(String^ name, Vector4 value);

		//static void SetGlobalTexture(String^ name, Texture& value);

		static void RecompileShaders();
		
		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);

	};
}