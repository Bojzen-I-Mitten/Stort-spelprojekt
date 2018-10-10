#pragma once
#pragma unmanaged
#include <thomas\resource\Material.h>
#pragma managed
#include "Resource.h"
using namespace Newtonsoft::Json;

namespace ThomasEngine
{
	ref class Shader;
	ref class Texture2D;
	[DataContractAttribute]
	public ref class Material : public Resource
	{
	private:
		Shader ^ m_shaderBeforePlay;
		Dictionary<String^, System::Object^>^ m_propertiesBeforePlay;
	internal:
		static List<Type^>^ GetKnownTypes();
		Material(thomas::resource::Material* ptr) : Resource(Utility::ConvertString(ptr->GetPath()), ptr) {};
	public:

		~Material()
		{
			delete m_nativePtr;
		}

		Material(Shader^ shader);

		Material(Material^ original);

		Material();


		void OnRename() override;

		void OnPlay() override
		{
#ifdef _EDITOR
			m_shaderBeforePlay = this->Shader;
			m_propertiesBeforePlay = this->EditorProperties;
#endif
		}

		void OnStop() override
		{
#ifdef _EDITOR
			this->Shader = m_shaderBeforePlay;
			this->EditorProperties = m_propertiesBeforePlay;
			m_shaderBeforePlay = nullptr;
			m_propertiesBeforePlay = nullptr;
#endif
		}


		property String^ Name
		{
			String^ get() override { 
				if (m_path->Length > 0)
					return System::IO::Path::GetFileNameWithoutExtension(m_path);
				else
				{
					return "Default Material";
				}
			}
			void set(String^ Name) {
				//Nothing here :)
			}
		};
		
		static property Material^ StandardMaterial
		{
			Material^ get();
		}
		
		void SetShaderPassEnabled(int index, bool enabled) { ((thomas::resource::Material*)m_nativePtr)->SetShaderPassEnabled(index, enabled); }
		void SetShaderPassEnabled(std::string name, bool enabled) { ((thomas::resource::Material*)m_nativePtr)->SetShaderPassEnabled(name, enabled); }

		Color GetColor(String^ name) { return Utility::Convert(((thomas::resource::Material*)m_nativePtr)->GetColor(Utility::ConvertString(name))); }
		void SetColor(String^ name, Color value) { ((thomas::resource::Material*)m_nativePtr)->SetColor(Utility::ConvertString(name), thomas::math::Color(value.r, value.g, value.b, value.a));  }

		float GetFloat(String^ name) { return ((thomas::resource::Material*)m_nativePtr)->GetFloat(Utility::ConvertString(name)); }
		void SetFloat(String^ name, float& value) { ((thomas::resource::Material*)m_nativePtr)->SetFloat(Utility::ConvertString(name), value); };

		int GetInt(String^ name) { return ((thomas::resource::Material*)m_nativePtr)->GetInt(Utility::ConvertString(name)); };
		void SetInt(String^ name, int& value) { ((thomas::resource::Material*)m_nativePtr)->SetInt(Utility::ConvertString(name),value); }

		Matrix GetMatrix(String^ name) { return  Utility::Convert(((thomas::resource::Material*)m_nativePtr)->GetMatrix(Utility::ConvertString(name))); }
		void SetMatrix(String^ name, Matrix value) { ((thomas::resource::Material*)m_nativePtr)->SetMatrix(Utility::ConvertString(name), Utility::Convert(value)); }

		Vector4 GetVector(String^ name) { return Utility::Convert(((thomas::resource::Material*)m_nativePtr)->GetVector(Utility::ConvertString(name))); }
		void SetVector(String^ name, Vector4 value) { ((thomas::resource::Material*)m_nativePtr)->SetVector(Utility::ConvertString(name), thomas::math::Vector4(value.x, value.y, value.z, value.w)); }

		Texture2D^ GetTexture2D(String^ name);
		void SetTexture2D(String^ name, Texture2D^ value);

		[IgnoreDataMemberAttribute]
		property Shader^ Shader
		{
			ThomasEngine::Shader^ get();
			void set(ThomasEngine::Shader^ value);
		}
#ifdef _EDITOR
		[IgnoreDataMemberAttribute]
		property Dictionary<String^, System::Object^>^ EditorProperties
		{
			Dictionary<String^, System::Object^>^ get();
			void set(Dictionary<String^, System::Object^>^ value);
		}
#endif
	private:
		Dictionary<String^, System::Object^>^ GetEditorProperties();

		/* Function called when material info is changed.
		*/
		void OnChange();

		/* Serialization
		*/
		[DataMemberAttribute(Order=0)]
		property ThomasEngine::Shader^ shader {
			ThomasEngine::Shader^ get();
			void set(ThomasEngine::Shader^ value);
		}
		[DataMemberAttribute(Order = 1)]
		[JsonPropertyAttribute(ObjectCreationHandling = ObjectCreationHandling::Replace)]
		property Dictionary<String^, System::Object^>^ properties
		{
			Dictionary<String^, System::Object^>^ get();
			void set(Dictionary<String^, System::Object^>^ value);
		}

	internal:
		[OnDeserializedAttribute]
		void OnDeserialized(StreamingContext c)
		{
		}
	};
}