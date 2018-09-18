
#include "Model.h"
#include "AudioClip.h"
#include "Material.h"
#include "Shader.h"
#include "texture\Texture2D.h"
#include "Resources.h"
#include "../Scene.h"
#include "..\object\GameObject.h"
namespace ThomasEngine
{
	Resource^ Resources::Load(String^ path)
	{
		String^ thomasPath = ConvertToThomasPath(path);
		if (resources->ContainsKey(thomasPath))
		{
			Resource^ obj = resources[thomasPath];
			return obj;
		}
		else
		{
			Resource^ obj;
			AssetTypes type = GetResourceAssetType(path);
			try {
				switch (type)
				{
				case AssetTypes::MODEL:
					obj = gcnew Model(path);
					break;
				case AssetTypes::TEXTURE2D:
					obj = gcnew Texture2D(path);
					break;
				case AssetTypes::SCENE:
					break;
				case AssetTypes::SHADER:
					obj = gcnew Shader(path);
					break;
				case AssetTypes::MATERIAL:
					obj = Deserialize<Material^>(path);
					break;
				case AssetTypes::SCRIPT:
					break;
				case AssetTypes::AUDIO_CLIP:
					obj = gcnew AudioClip(path);
					break;
				case AssetTypes::UNKNOWN:
					break;
				default:
					break;
				}
			}
			catch (SerializationException^ e) {
				std::string error = "Error creating resource from file: " + Utility::ConvertString(path) + " \nError: Serialization failed";
				LOG(error);
			}
			catch (Exception^ e) {
				std::string error = "Error creating resource from file: " + Utility::ConvertString(path) + " \nError: " + Utility::ConvertString(e->Message);
				LOG(error);
				//Debug::Log("Failed to create resource from file. Filename: " + path + " \nError: " + e->Message);
			}
			
			if (obj != nullptr)
			{
				resources[thomasPath] = obj;
			}
			return obj;
		}
		
	}
	void Resources::SavePrefab(GameObject ^ gameObject, String ^ path)
	{
		path = Application::currentProject->assetPath + "\\" + path;

		GameObject::SerializeGameObject(path, gameObject);

	}
	GameObject ^ Resources::LoadPrefab(String^ path)
	{
		for each(GameObject^ gObj in GameObject::GetAllGameObjects(true))
		{
			if (gObj->prefabPath == path)
				return gObj;
		}
		
		IO::FileStream^ fileStream = IO::File::OpenRead(path);
		GameObject^ prefab = GameObject::DeSerializeGameObject(fileStream);
		fileStream->Close();
		if(prefab)
			prefab->prefabPath = path;
		return prefab;
	}
	Resources::AssetTypes Resources::GetResourceAssetType(Type ^ type)
	{
		
		if (type == AudioClip::typeid)
		{
			return AssetTypes::AUDIO_CLIP;
		}
		else if (type == Model::typeid)
		{
			return AssetTypes::MODEL;
		}
		else if (type == Material::typeid)
		{
			return AssetTypes::MATERIAL;
		}
		else if (type == Shader::typeid)
		{
			return AssetTypes::SHADER;
		}
		else if (type == Texture2D::typeid)
		{
			return AssetTypes::TEXTURE2D;
		}
		else
		{
			return AssetTypes::UNKNOWN;
		}
	}
	Resource ^ Resources::Find(String ^ path)
	{
		String^ thomasPath = ConvertToThomasPath(path);
		if (resources->ContainsKey(thomasPath))
		{
			return resources[thomasPath];
		}
		return nullptr;
	}
	void Resources::RenameResource(String ^ oldPath, String ^ newPath)
	{
		String^ thomasPathOld = ConvertToThomasPath(oldPath);
		String^ thomasPathNew = ConvertToThomasPath(oldPath);
		if (resources->ContainsKey(thomasPathOld))
		{
			Object^ lock = Scene::CurrentScene->GetGameObjectsLock();

			System::Threading::Monitor::Enter(lock);
			Resource^ resource = resources[thomasPathOld];
			resources->Remove(thomasPathOld);
			resources[thomasPathNew] = resource;
			if (resource)
				resource->Rename(newPath);

			System::Threading::Monitor::Exit(lock);
		}
	}
}