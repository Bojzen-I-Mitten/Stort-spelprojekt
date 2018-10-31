#pragma unmanaged
#include <thomas\resource\Resource.h>
#pragma managed
#include "Model.h"
#include "Material.h"
#include "AudioClip.h"
#include "Shader.h"
#include "Animation.h"
#include "texture\Texture2D.h"
#include "texture\TextureCube.h"
#include "Resources.h"
#include "../Scene.h"
#include "../Application.h"
#include "../Project.h"
#include "../Debug.h"
#include "../object/GameObject.h"
#include "../serialization/Serializer.h"
#include "../Time.h"
#include "Font.h"
using namespace System::Threading;
namespace ThomasEngine
{

	void Resources::OnPlay()
	{
		for each (Resource^ resource in resources->Values)
		{
			resource->OnPlay();
		}
	}
	void Resources::OnStop()
	{
		for each (Resource^ resource in resources->Values)
		{
			resource->OnStop();
		}
	}



#pragma region Path

	String^ Resources::ConvertToThomasPath(String^ value) {
		if (value->Contains("%THOMAS_ASSETS%") || value->Contains("%THOMAS_DATA%"))
			return value;
		if (value->Contains(Application::editorAssets)) value = value->Replace(Application::editorAssets, "%THOMAS_DATA%");
		else if (Application::currentProject) value = value->Replace(Application::currentProject->assetPath, "%THOMAS_ASSETS%");
		else {
			value = System::IO::Path::GetFullPath(value);
			value = ConvertToThomasPath(value);
		}

		return value;
	}
	String^ Resources::ConvertToRealPath(String^ value) {
		value = value->Replace("%THOMAS_DATA%", Application::editorAssets);
		value = value->Replace("%THOMAS_ASSETS%", Application::currentProject->assetPath);
		value = System::IO::Path::GetFullPath(value);
		return value;
	}
	String^ Resources::GetUniqueName(String^ path)
	{
		String^ extension = IO::Path::GetExtension(path);
		String^ modifier = "";
		path = IO::Path::GetDirectoryName(path) + "\\" + IO::Path::GetFileNameWithoutExtension(path);
		int i = 0;
		while (IO::File::Exists(path + modifier + extension))
		{
			i++;
			modifier = "(" + i + ")";
		}
		path = path + modifier + extension;
		return path;
	}
#pragma endregion

#pragma region Serialization (Create/Store)


		bool Resources::CreateResource(Resource^ resource, String^ path)
		{
			path = GetUniqueName(Application::currentProject->assetPath + "\\" + path);
			if (resource->GetType() == Material::typeid)
			{
				resource->Rename(path);
				Serializer::SerializeMaterial((Material^)resource, path);
				return true;
			}
			return false;
			
		}
		
#pragma endregion

#pragma region Type

		Resources::AssetTypes Resources::GetResourceAssetType(String^ path)
		{
			String^ extension = System::IO::Path::GetExtension(path);
			if (extension->Length == 0)
				return AssetTypes::UNKNOWN;
			extension = extension->Remove(0, 1)->ToLower();
			if (extension == "fx")
			{
				return AssetTypes::SHADER;
			}
			else if (extension == "cs")
			{
				return AssetTypes::SCRIPT;
			}
			else if (extension == "tds")
			{
				return AssetTypes::SCENE;
			}
			else if (extension == "wav" || extension == "mp3")
			{
				return AssetTypes::AUDIO_CLIP;
			}
			else if (extension == "obj" || extension == "fbx")
			{
				return AssetTypes::MODEL;
			}
			else if (extension == "anim" || extension == "dae")
			{
				return AssetTypes::ANIMATION;
			}
			else if (extension == "mat")
			{
				return AssetTypes::MATERIAL;
			}
			else if (extension == "bmp" || extension == "jpg" || extension == "png" || extension == "gif" || extension == "tif")
			{
				return AssetTypes::TEXTURE2D;
			}
			else if (extension == "dds")
			{
				return AssetTypes::TEXTURE3D;
			}
			else if (extension == "prefab")
				return AssetTypes::PREFAB;
			else if (extension == "spritefont")
				return AssetTypes::FONT;
			else
			{
				return AssetTypes::UNKNOWN;
			}
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
			else if (type == Animation::typeid)
			{
				return AssetTypes::ANIMATION;
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
			else if (type == Font::typeid)
				return AssetTypes::FONT;
			else
			{
				return AssetTypes::UNKNOWN;
			}
		}

#pragma endregion

#pragma region Load/Unload/Fetch

		Resource^ Resources::LoadThomasPath(String^ thomasPath)
		{
			String^ sysPath = ConvertToRealPath(thomasPath);
			return Load(sysPath, thomasPath);
		}
		Resource^ Resources::LoadSysPath(String^ sysPath)
		{
			String^ thomasPath = ConvertToThomasPath(sysPath);
			return Load(sysPath, thomasPath);
		}
		Resource^ Resources::Load(String^ path, String^ thomasPath)
		{
			if (resources->ContainsKey(thomasPath))
			{
				Resource^ obj = resources[thomasPath];
				return obj;
			}
			else
			{
				float startTime = Time::ElapsedTime;
				
				Resource^ obj;
				AssetTypes type = GetResourceAssetType(path);
				try {
					if (!System::IO::File::Exists(path))
						throw gcnew System::IO::FileNotFoundException("File not found");
					switch (type)
					{
					case AssetTypes::MODEL:
						obj = gcnew Model(path);
						break;
					case AssetTypes::TEXTURE2D:
						obj = gcnew Texture2D(path);
						break;
					case AssetTypes::TEXTURE3D:
						obj = gcnew TextureCube(path);
						break;
					case AssetTypes::SCENE:
						break;
					case AssetTypes::SHADER:
						obj = gcnew Shader(path);
						break;
					case AssetTypes::ANIMATION:
						obj = gcnew Animation(path);
						break;
					case AssetTypes::MATERIAL:
						obj = Serializer::DeserializeMaterial(path);
						break;
					case AssetTypes::SCRIPT:
						break;
					case AssetTypes::AUDIO_CLIP:
						obj = gcnew AudioClip(path);
						break;
					case AssetTypes::FONT:
						obj = gcnew Font(path);
						break;
					case AssetTypes::UNKNOWN:
						break;
					default:
						break;
					}

					if (obj != nullptr)
					{
						resources[thomasPath] = obj;
					}
				}
				catch (Exception^ e) {

					String^ error = "Error loading " + type.ToString() + " resource from: " + path + " Error: " + e->Message;

					Debug::LogError(error);
					obj = LoadErrorResource(type);
					if(obj == nullptr)
						Debug::LogWarning("Warning Default Object does not exist of type: " + type.ToString());
				}

				//Debug::Log(path + " (" + (Time::ElapsedTime - startTime).ToString("0.00") + ")");

				return obj;
			}

		}
#pragma region PreFab

		void Resources::SavePrefab(GameObject ^ gameObject, String ^ path)
		{
			if (gameObject->prefabPath)
				path = gameObject->prefabPath;
			else
				path = Application::currentProject->assetPath + "\\" + path;

			Serializer::SerializeGameObject(gameObject, path);

		}

		GameObject ^ Resources::LoadPrefab(String^ path) {
			return LoadPrefab(path, false);
		}

		GameObject ^ Resources::LoadPrefab(String^ path, bool forceInstantiate)
		{
			if (!forceInstantiate) {
				for each(GameObject^ gObj in GameObject::GetAllGameObjects(true))
				{
					if (gObj->prefabPath == path)
						return gObj;
				}
			}

			try {
				GameObject^ prefab = Serializer::DeserializeGameObject(path);
				if (prefab)
					prefab->prefabPath = path;
				return prefab;
			}
			catch (Exception^ e) {
				String^ msg = "Failed to load prefab: " + path + " error: " + e->Message;
				Debug::LogError(msg);
				return nullptr;
			}

		}

#pragma endregion
		generic<typename T>
			where T : Resource
			T Resources::Load(String^ path)
			{
				String^ thomasPath = ConvertToThomasPath(path);
				if (resources->ContainsKey(thomasPath))
				{
					Resource^ obj = resources[thomasPath];
					if (obj->GetType() == T::typeid)
						return (T)obj;
					else
					{
						return T();
						//error blablablabla.
					}
				}
				else
				{
					T resource = (T)Activator::CreateInstance(T::typeid, path);
					resources[thomasPath] = resource;
					return resource;
				}
			}

			void Resources::Unload(Resource^ resource) {
				if (Find(resource->m_path))
				{
					resources->Remove(System::IO::Path::GetFullPath(resource->m_path));
				}
			}

			void Resources::LoadAll(String^ path)
			{
				array<String^>^ directories = IO::Directory::GetDirectories(path);
				array<String^>^ files = IO::Directory::GetFiles(path);
				for each(String^ dir in directories)
				{
					LoadAll(dir);
				}
				for each(String^ file in files)
				{
					LoadSysPath(file);
				}
			}

			void Resources::UnloadAll()
			{
				for each(String^ resource in resources->Keys)
					delete resources[resource];
			}
#pragma endregion

#pragma region Find/Set/Get

			Resource^ Resources::FindResourceFromNativePtr(thomas::resource::Resource* nativePtr)
			{
				if (nativePtr == nullptr)
					return nullptr;
				for each(Resource^ resource in resources->Values)
				{
					if (resource->m_nativePtr == nativePtr)
						return resource;
				}
				return nullptr;
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

			List<Resource^>^ Resources::GetResourcesOfType(Type^ type)
			{
				List<Resource^>^ list = gcnew List<Resource^>();
				for each (Resource^ resource in resources->Values)
				{
					if (resource->GetType() == type)
						list->Add(resource);
				}
				return list;
			}
			Resource ^ Resources::LoadErrorResource(AssetTypes type)
			{
				Resource^ obj = nullptr;
				switch (type)
				{
				case AssetTypes::MODEL:
					//obj = gcnew Model(path); Error model
					break;
				case AssetTypes::TEXTURE2D:
					//obj = gcnew Texture2D(path); Error texture
					break;
				case AssetTypes::SCENE:
					break;
				case AssetTypes::SHADER:
					//obj = Shader::Find() Failed shader
					break;
				case AssetTypes::MATERIAL:
					return Material::StandardMaterial;
					// obj = Deserialize<Material^>(path); Failed material
					break;
				case AssetTypes::SCRIPT:
					break;
				case AssetTypes::AUDIO_CLIP:
					// obj = gcnew AudioClip(path); Failed audio?
					break;
				case AssetTypes::UNKNOWN:
				default:
					break;
				}
				return obj;
			}
			void Resources::RenameResource(String ^ oldPath, String ^ newPath)
			{
				String^ thomasPathOld = ConvertToThomasPath(oldPath);
				String^ thomasPathNew = ConvertToThomasPath(newPath);
				if (resources->ContainsKey(thomasPathOld))
				{
					Object^ lock = ThomasWrapper::CurrentScene->GetGameObjectsLock();

					System::Threading::Monitor::Enter(lock);
					Resource^ resource = resources[thomasPathOld];
					resources->Remove(thomasPathOld);
					resources[thomasPathNew] = resource;
					if (resource)
						resource->Rename(newPath);

					System::Threading::Monitor::Exit(lock);
				}
			}
			generic<typename T>
				where T : Resource
			List<T>^ Resources::GetResourcesOfType()
			{
					return (List<T>^)System::Linq::Enumerable::OfType<T>(resources->Values);
			}
#pragma endregion


}