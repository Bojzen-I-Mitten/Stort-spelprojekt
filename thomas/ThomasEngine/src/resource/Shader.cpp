
#include "Shader.h"
#include "Resources.h"
namespace ThomasEngine {
	Shader^ Shader::Find(String^ name) {
		thomas::resource::Shader* nativePtr = thomas::resource::Shader::FindByName(Utility::ConvertString(name));
		if (nativePtr)
		{
			ThomasEngine::Resource^ shader = ThomasEngine::Resources::FindResourceFromNativePtr(nativePtr);
			if (shader)
				return (ThomasEngine::Shader^)shader;
			else
				return gcnew ThomasEngine::Shader(nativePtr);
		}
		else
			return nullptr;
	}
}