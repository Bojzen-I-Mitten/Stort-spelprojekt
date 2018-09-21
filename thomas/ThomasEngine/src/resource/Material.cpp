
#include "Material.h"
#include "../ThomasManaged.h"

namespace ThomasEngine {

	void Material::Shader::set(ThomasEngine::Shader^ value)
	{
		if (m_nativePtr)
			((thomas::resource::Material*)m_nativePtr)->SetShader((thomas::resource::Shader*)value->m_nativePtr);
		else
		{
			m_nativePtr = new thomas::resource::Material(Utility::ConvertString(m_path));
			((thomas::resource::Material*)m_nativePtr)->SetShader((thomas::resource::Shader*)value->m_nativePtr);
		}

	if (m_loaded && !ThomasWrapper::IsPlaying() && this->Shader != value)
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


	Dictionary<String^, System::Object^>^ Material::GetEditorProperties()
	{
		Dictionary<String^, System::Object^>^ properties = gcnew Dictionary<String^, System::Object^>();
		for (auto& prop : ((thomas::resource::Material*)m_nativePtr)->GetEditorProperties())
		{
			String^ name = Utility::ConvertString(prop.first);
			System::Object^ value;
			switch (prop.second->GetType())
			{
			case thomas::resource::shaderproperty::ShaderProperty::Type::SCALAR_BOOL:

				break;
			case thomas::resource::shaderproperty::ShaderProperty::Type::SCALAR_FLOAT:
				value = GetFloat(name);
				break;
			case thomas::resource::shaderproperty::ShaderProperty::Type::SCALAR_INT:
				value = GetInt(name);
				break;
			case thomas::resource::shaderproperty::ShaderProperty::Type::VECTOR:
				value = GetVector(name);
				break;
			case thomas::resource::shaderproperty::ShaderProperty::Type::COLOR:
				value = GetColor(name);
				break;
			case thomas::resource::shaderproperty::ShaderProperty::Type::MATRIX:
				value = GetMatrix(name);
				break;
			case thomas::resource::shaderproperty::ShaderProperty::Type::TEXTURE2D:
				value = GetTexture2D(name);
				break;
			default:
				break;
			}
			properties->Add(name, value);
		}
		return properties;
	}
	List<Type^>^ ThomasEngine::Material::GetKnownTypes()
	{
		List<Type^>^ types = gcnew List<Type^>();

		types->Add(Vector2::typeid);
		types->Add(Vector3::typeid);
		types->Add(Vector4::typeid);
		types->Add(Color::typeid);
		types->Add(Material::typeid);
		types->Add(Texture::typeid);
		types->Add(Texture2D::typeid);
		types->Add(ThomasEngine::Shader::typeid);
		return types;
	}
}