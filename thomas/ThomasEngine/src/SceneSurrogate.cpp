#include "SceneSurrogate.h"
#include "resource\Material.h"
#include "resource\texture\Texture2D.h"
#include "object\GameObject.h"
#include "object\Component.h"
#include "resource\Resources.h"
using namespace System;
namespace ThomasEngine
{

	System::Type ^ SceneSurrogate::GetDataContractType(System::Type ^ type)
	{
		if (Resource::typeid->IsAssignableFrom(type) && rootType != type)
		{
			return SceneResource::typeid;
		}
		else
		{
			return type;
		}
		return type;
	}

	System::Object ^ SceneSurrogate::GetObjectToSerialize(System::Object ^obj, System::Type ^targetType)
	{
		if (Resource::typeid->IsAssignableFrom(obj->GetType()) && rootType != obj->GetType())
		{
			Resource^ resource = (Resource^)obj;
			return gcnew SceneResource(resource->GetAssetRelativePath());
		}
		else if (obj->GetType() == GameObject::typeid) {
			GameObject^ gameObject = (GameObject^)obj;
			if (gameObject->prefabPath)
				return gcnew SceneResource(Resources::ConvertToThomasPath(gameObject->prefabPath));
		}
		else if (Component::typeid->IsAssignableFrom(obj->GetType())) {
			Component^ component = (Component^)obj;
			if (component->gameObject->prefabPath)
				return gcnew SceneResource(Resources::ConvertToThomasPath(component->gameObject->prefabPath));
		}
		return obj;
	}

	System::Object ^ SceneSurrogate::GetDeserializedObject(System::Object ^obj, System::Type ^targetType)
	{
		if (obj->GetType() == SceneResource::typeid)
		{
			SceneResource^ sceneResource = (SceneResource^)obj;
			if (sceneResource->path == "")
			{
				if (targetType == Material::typeid)
					return Material::StandardMaterial;
			}
			else if (sceneResource->path == "White Texture")
				return Texture2D::whiteTexture;
			else if (sceneResource->path == "Black Texture")
				return Texture2D::whiteTexture;

			else if (targetType == GameObject::typeid)
				return Resources::LoadPrefab(Resources::ConvertToRealPath(sceneResource->path));
			else if (Component::typeid->IsAssignableFrom(targetType)) {
				return Resources::LoadPrefab(Resources::ConvertToRealPath(sceneResource->path))->GetComponent(targetType);
			}
			else {
				return Resources::LoadThomasPath(sceneResource->path);
			}
		}
		return obj;
	}

	System::Object ^ SceneSurrogate::GetCustomDataToExport(System::Reflection::MemberInfo ^memberInfo, System::Type ^dataContractType)
	{
		throw gcnew NotSupportedException("unused");
	}

	System::Object ^ SceneSurrogate::GetCustomDataToExport(System::Type ^clrType, System::Type ^dataContractType)
	{
		throw gcnew NotSupportedException("unused");
	}

	void SceneSurrogate::GetKnownCustomDataTypes(System::Collections::ObjectModel::Collection<System::Type ^> ^customDataTypes)
	{
		throw gcnew NotSupportedException("unused");
	}

	System::Type ^ SceneSurrogate::GetReferencedTypeOnImport(System::String ^typeName, System::String ^typeNamespace, System::Object ^customData)
	{
		throw gcnew NotSupportedException("unused");
	}

	System::CodeDom::CodeTypeDeclaration ^ SceneSurrogate::ProcessImportedType(System::CodeDom::CodeTypeDeclaration ^typeDeclaration, System::CodeDom::CodeCompileUnit ^compileUnit)
	{
		throw gcnew NotSupportedException("unused");
	}
}
