#include "Converters.h"
#include "../resource/Resource.h"
#include "../resource/Resources.h"
#include "../object/GameObject.h"
#include "../object/Component.h"

#include "../resource/Material.h"
#include "../resource/texture/Texture2D.h"

#include "Serializer.h"
namespace ThomasEngine
{
	void ResourceConverter::WriteJson(Newtonsoft::Json::JsonWriter ^writer, System::Object ^value, Newtonsoft::Json::JsonSerializer ^serializer)
	{
		Resource^ resource = (Resource^)value;
		JObject^ jo = gcnew JObject();
		jo->Add("resourcePath", resource->GetAssetRelativePath());
		jo->WriteTo(writer);
	}

	System::Object ^ ResourceConverter::ReadJson(Newtonsoft::Json::JsonReader ^reader, System::Type ^objectType, System::Object ^existingValue, Newtonsoft::Json::JsonSerializer ^serializer)
	{
		JObject^ jo = JObject::Load(reader);
		String^ path = jo->Value<String^>("resourcePath");
		
		if (path == "")
			return Material::StandardMaterial;
		else if (path == "White Texture")
			return Texture2D::whiteTexture;
		else if (path == "Black Texture")
			return Texture2D::whiteTexture;
		else if (path == "Normal Texture")
			return Texture2D::normalTexture;
		else
			return Resources::LoadThomasPath(path);
	}

	bool ResourceConverter::CanConvert(System::Type ^objectType)
	{
		if (Serializer::rootType == objectType)
			return false;
		else
			return Resource::typeid->IsAssignableFrom(objectType);
	}

	void PrefabConverter::WriteJson(Newtonsoft::Json::JsonWriter ^writer, System::Object ^value, Newtonsoft::Json::JsonSerializer ^serializer)
	{
		JObject^ jo = gcnew JObject();
		GameObject^ gameObject = (GameObject^)value;

		if (gameObject->prefabPath) {
			jo->Add("prefabPath", Resources::ConvertToThomasPath(gameObject->prefabPath));
			
		}
		else
		{
			for each(System::Reflection::PropertyInfo^ prop in value->GetType()->GetProperties())
			{
				if (prop->CanRead && prop->CanWrite && prop->SetMethod)
				{
					Object^ propVal = prop->GetValue(value, nullptr);
					if (propVal != nullptr)
					{
						String^ info = prop->Name;
						jo->Add(info, JToken::FromObject(propVal, serializer));
					}
				}
			}
		}
		jo->WriteTo(writer);

	}

	System::Object ^ PrefabConverter::ReadJson(Newtonsoft::Json::JsonReader ^reader, System::Type ^objectType, System::Object ^existingValue, Newtonsoft::Json::JsonSerializer ^serializer)
	{
		if (reader->TokenType == JsonToken::Null)
			return nullptr;
		JObject^ jo = JObject::Load(reader);

		if (jo->ContainsKey("prefabPath"))
		{
			
			String^ path = jo->Value<String^>("prefabPath");
			return Resources::LoadPrefab(Resources::ConvertToRealPath(path));
		}
		else
		{
			existingValue = existingValue ? existingValue : serializer->ContractResolver->ResolveContract(objectType)->DefaultCreator();
			serializer->Populate(jo->CreateReader(), existingValue);
			return existingValue;
		}
 	}

	bool PrefabConverter::CanConvert(System::Type ^objectType)
	{
		return objectType == GameObject::typeid;
	}



	System::Object ^ ComponentConverter::ReadJson(Newtonsoft::Json::JsonReader ^ reader, System::Type ^ objectType, System::Object ^ existingValue, Newtonsoft::Json::JsonSerializer ^ serializer)
	{

		JObject^ jo = JObject::Load(reader);
		Type^ correctType = objectType;
	
		if (jo->ContainsKey("$ref"))
		{
			System::Object^ o = serializer->ReferenceResolver->ResolveReference(serializer, jo->Value<String^>("$ref"));
			return o;
		}
		if (jo->ContainsKey("$type")) {
			String^ typeName = jo->Value<String^>("$type")->Split(',')[0];

			for each(Type^ type in Component::GetAllComponentTypes())
			{
				if (typeName == type->FullName)
				{
					correctType = type;
				}
			}
		}

		existingValue = existingValue ? existingValue : serializer->ContractResolver->ResolveContract(correctType)->DefaultCreator();
		serializer->Populate(jo->CreateReader(), existingValue);
		return existingValue;
	}

	bool ComponentConverter::CanConvert(System::Type ^ objectType)
	{
		return ThomasEngine::Component::typeid->IsAssignableFrom(objectType);
	}

	void ComponentConverter::WriteJson(Newtonsoft::Json::JsonWriter ^writer, System::Object ^value, Newtonsoft::Json::JsonSerializer ^serializer)
	{
		throw gcnew System::NotImplementedException();
	}

}
