
#pragma unmanaged
#include <thomas/graphics/Renderer.h>
#pragma managed
#include "Shader.h"
#include "Resources.h"
namespace ThomasEngine
{
	Shader::Shader(thomas::resource::Shader* ptr) 
		: Resource(Utility::ConvertString(ptr->GetPath()), ptr) 
	{}
	Shader::Shader(String^ path) : 
		Resource(path, thomas::graphics::Renderer::Instance()->getShaderList().CreateShader(Utility::ConvertString(path)))
	{}

	Shader::~Shader() 
	{
		thomas::resource::Shader* s = dynamic_cast<thomas::resource::Shader*>(m_nativePtr);
		if (s)
		{
			thomas::graphics::Renderer::Instance()->getShaderList().rmv(s);
			m_nativePtr = nullptr;
		}
	}


	Shader^ Shader::Find(String^ name) {
		thomas::resource::Shader* nativePtr = thomas::graphics::Renderer::Instance()->getShaderList().FindByName(Utility::ConvertString(name));
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

	//static void SetGlobalTexture(String^ name, Texture& value);

	void Shader::RecompileShaders() { thomas::graphics::Renderer::Instance()->getShaderList().QueueRecompile(); }

	[OnDeserializedAttribute]
	void Shader::OnDeserialized(StreamingContext c)
	{
		m_nativePtr = thomas::graphics::Renderer::Instance()->getShaderList().CreateShader(Utility::ConvertString(Path));
	}

}