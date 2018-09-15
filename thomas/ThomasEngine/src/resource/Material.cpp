

#pragma unmanaged
#include <thomas\resource\ShaderProperty\shaderProperties.h>
#pragma managed

#include "Material.h"
#include "../ThomasManaged.h"
#include "Shader.h"
#include "texture\Texture2D.h"
#include "Resources.h"
namespace ThomasEngine {

	Material::Material(ThomasEngine::Shader^ shader) : Resource(shader->Name + " Material.mat", new thomas::resource::Material((thomas::resource::Shader*)shader->m_nativePtr))
	{
		m_loaded = true;
	}

	Material^ Material::StandardMaterial::get() {
		return gcnew Material(thomas::resource::Material::GetStandardMaterial());
	}

	Shader^ Material::Shader::get()
	{
		thomas::resource::Shader* nativePtr = ((thomas::resource::Material*)m_nativePtr)->GetShader();
		ThomasEngine::Resource^ shader = ThomasEngine::Resources::FindResourceFromNativePtr(nativePtr);
		if (shader)
			return (ThomasEngine::Shader^)shader;
		else
			return gcnew ThomasEngine::Shader(nativePtr);
	}

	void Material::Shader::set(ThomasEngine::Shader^ value)
	{
		if (m_nativePtr)
			((thomas::resource::Material*)m_nativePtr)->SetShader((thomas::resource::Shader*)value->m_nativePtr);
		else
		{
			m_nativePtr = new thomas::resource::Material(Utility::ConvertString(m_path));
			((thomas::resource::Material*)m_nativePtr)->SetShader((thomas::resource::Shader*)value->m_nativePtr);
		}

		if (m_loaded && !ThomasWrapper::IsPlaying())
		{
			ThomasEngine::Resources::SaveResource(this);
		}
	}

	void Material::EditorProperties::set(Dictionary<String^, System::Object^>^ value)
	{

		for each(String^ key in value->Keys)
		{
			System::Object^ prop = value[key];
			Type^ t = prop->GetType();
			if (t == Vector4::typeid)
			{
				Vector4 v = (Vector4)prop;
				SetVector(key, v);
			}
			else if (t == Color::typeid)
			{
				Color v = (Color)prop;
				SetColor(key, v);
			}
			else if (t == Texture2D::typeid)
			{
				Texture2D^ v = (Texture2D^)prop;
				SetTexture2D(key, v);
			}
			else if (t == System::Single::typeid)
			{
				//SetRaw(key, &prop);
			}
		}
		if (m_loaded && !ThomasWrapper::IsPlaying())
		{
			ThomasEngine::Resources::SaveResource(this);
		}
	}

	Texture2D^ Material::GetTexture2D(String^ name)
	{
		thomas::resource::Texture2D* nativePtr = ((thomas::resource::Material*)m_nativePtr)->GetTexture2D(Utility::ConvertString(name));
		ThomasEngine::Resource^ texture = ThomasEngine::Resources::FindResourceFromNativePtr(nativePtr);
		if (texture)
			return (ThomasEngine::Texture2D^)texture;
		else
			return gcnew ThomasEngine::Texture2D(nativePtr);
	}

	void Material::SetTexture2D(String^ name, Texture2D^ value)
	{ 
		((thomas::resource::Material*)m_nativePtr)->SetTexture2D(Utility::ConvertString(name), (thomas::resource::Texture2D*)value->m_nativePtr); 
	}

	Dictionary<String^, System::Object^>^ Material::GetEditorProperties()
	{
		Dictionary<String^, System::Object^>^ properties = gcnew Dictionary<String^, System::Object^>();
		for (auto& prop : ((thomas::resource::Material*)m_nativePtr)->GetEditorProperties())
		{
			String^ name = Utility::ConvertString(prop.first);
			System::Object^ value;
			switch (prop.second->GetType())
			{
			case thomas::resource::shaderProperty::ShaderProperty::Type::SCALAR_BOOL:

				break;
			case thomas::resource::shaderProperty::ShaderProperty::Type::SCALAR_FLOAT:
				value = GetFloat(name);
				break;
			case thomas::resource::shaderProperty::ShaderProperty::Type::SCALAR_INT:
				value = GetInt(name);
				break;
			case thomas::resource::shaderProperty::ShaderProperty::Type::VECTOR:
				value = GetVector(name);
				break;
			case thomas::resource::shaderProperty::ShaderProperty::Type::COLOR:
				value = GetColor(name);
				break;
			case thomas::resource::shaderProperty::ShaderProperty::Type::MATRIX:
				value = GetMatrix(name);
				break;
			case thomas::resource::shaderProperty::ShaderProperty::Type::TEXTURE2D:
				value = GetTexture2D(name);
				break;
			default:
				break;
			}
			properties->Add(name, value);
		}
		return properties;
	}

}

