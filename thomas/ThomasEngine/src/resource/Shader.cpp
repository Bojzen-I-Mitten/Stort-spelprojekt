
#include "Shader.h"
#include "Resources.h"
namespace ThomasEngine
{
	Shader::Shader(thomas::resource::Shader* ptr) 
		: Resource(Utility::ConvertString(ptr->GetPath()), ptr) 
	{}
	Shader::Shader(String^ path) : 
		Resource(path, thomas::resource::Shader::CreateShader(Utility::ConvertString(path))) 
	{}


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

	void Shader::SetGlobalColor(String^ name, Color value) { thomas::resource::Shader::SetGlobalColor(Utility::ConvertString(name), Utility::Convert(value)); };
	void Shader::SetGlobalFloat(String^ name, float value) { thomas::resource::Shader::SetGlobalFloat(Utility::ConvertString(name), value); };;
	void Shader::SetGlobalInt(String^ name, int value) { thomas::resource::Shader::SetGlobalInt(Utility::ConvertString(name), value); };;
	void Shader::SetGlobalMatrix(String^ name, Matrix value) { thomas::resource::Shader::SetGlobalMatrix(Utility::ConvertString(name), Utility::Convert(value)); };;
	void Shader::SetGlobalVector(String^ name, Vector4 value) { thomas::resource::Shader::SetGlobalVector(Utility::ConvertString(name), thomas::math::Vector4(value.x, value.y, value.z, value.w)); };;

	//static void SetGlobalTexture(String^ name, Texture& value);

	void Shader::RecompileShaders() { thomas::resource::Shader::QueueRecompile(); }

	[OnDeserializedAttribute]
	void Shader::OnDeserialized(StreamingContext c)
	{
		m_nativePtr = thomas::resource::Shader::CreateShader(Utility::ConvertString(m_path));
	}

}