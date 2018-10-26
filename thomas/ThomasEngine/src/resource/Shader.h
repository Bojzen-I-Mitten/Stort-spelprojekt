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
		virtual ~Shader();

	public:
		

		static Shader^ Find(String^ name);

		//static void SetGlobalTexture(String^ name, Texture& value);

		static void RecompileShaders();
		
		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c);

	};
}