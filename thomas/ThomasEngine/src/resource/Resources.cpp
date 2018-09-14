
#include "Model.h"
#include "AudioClip.h"
#include "Material.h"
#include "Shader.h"
#include "Animation.h"
#include "texture\Texture2D.h"
#include "Resources.h"
#include "../Scene.h"
#include "..\object\GameObject.h"
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

#pragma region PreFab

	void Resources::SavePrefab(GameObject ^ gameObject, String ^ path)
	{
		path = Application::currentProject->assetPath + "\\" + path;

		GameObject::SerializeGameObject(path, gameObject);

	}

	GameObject ^ Resources::LoadPrefab(String^ path)
	{
		IO::FileStream^ fileStream = IO::File::OpenRead(path);
		GameObject^ prefab = GameObject::DeSerializeGameObject(fileStream);
		fileStream->Close();
		return prefab;
	}

#pragma endregion

#pragma region Path

		String^ Resources::ConvertToThomasPath(String^ value) {
			value = System::IO::Path::GetFullPath(value);
			if (value->Contains(Application::editorAssets)) return value->Replace(Application::editorAssets, "%THOMAS_DATA%");
			else return value->Replace(Application::currentProject->assetPath, "%THOMAS_ASSETS%");
		}
		String^ Resources::ConvertToRealPath(String^ value) {
			value = value->Replace("%THOMAS_DATA%", Application::editorAssets);
			value = value->Replace("%THOMAS_ASSETS%", Application::currentProject->assetPath);
			return value;
		}
		String^ Resources::GetUniqueName(String^ path) {
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

#pragma region Serialization (Create/Save)

		List<Type^>^ Resources::getKnownTypes() {
			List<Type^>^ knownTypes = gcnew List<Type^>();
			knownTypes->AddRange(System::Reflection::Assembly::GetAssembly(Resource::typeid)->ExportedTypes);
			knownTypes->AddRange(System::Reflection::Assembly::GetAssembly(Color::typeid)->ExportedTypes);
			return knownTypes;
		}

		generic<typename T>
			where T : Resource
			T Resources::Deserialize(String^ path)
			{

				Monitor::Enter(resourceLock);
				using namespace System::Runtime::Serialization;
				DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
				serializserSettings->PreserveObjectReferences = true;
				serializserSettings->KnownTypes = getKnownTypes();
				DataContractSerializer^ serializer = gcnew DataContractSerializer(T::typeid, serializserSettings);

				Xml::XmlReader^ file = Xml::XmlReader::Create(path);
				T resource = (T)serializer->ReadObject(file);
				file->Close();
				resource->Rename(path);
				Monitor::Exit(resourceLock);
				return resource;


			}
			bool Resources::CreateResource(Resource^ resource, String^ path)
			{
				path = GetUniqueName(Application::currentProject->assetPath + "\\" + path);
				Monitor::Enter(resourceLock);
				using namespace System::Runtime::Serialization;


				Xml::XmlWriter^ file;
				std::string err;
				try {
					// Create xml writer
					Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
					settings->Indent = true;
					// Create file
					System::IO::FileInfo^ fi = gcnew System::IO::FileInfo(path);
					fi->Directory->Create();
					file = Xml::XmlWriter::Create(path, settings);
					try {
						// Serialization Settings
						DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
						serializserSettings->PreserveObjectReferences = true;
						serializserSettings->KnownTypes = getKnownTypes();
						// Write File
						DataContractSerializer^ serializer = gcnew DataContractSerializer(resource->GetType(), serializserSettings);
						serializer->WriteObject(file, resource);
						// Success: Append resource
						resources[System::IO::Path::GetFullPath(path)] = resource;
						resource->Rename(path);	// Set file name
					}
					catch (Exception^ e) {
						err = std::string("Failed to serialize resource file, at path:" + Utility::ConvertString(path) + ". With message:\n" + Utility::ConvertString(e->Message));
						LOG(err);
						return false;
					}
				}
				catch (Exception^ e) {
					err = std::string("Failed to create resource file, at path:" + Utility::ConvertString(path) + ". With message:\n" + Utility::ConvertString(e->Message));
					LOG(err);
					return false;
				}
				finally{
					// Close file stream
					file->Close();
					Monitor::Exit(resourceLock);
				}
				return true;
			}
			void Resources::SaveResource(Resource^ resource)
			{

				Monitor::Enter(resourceLock);
				using namespace System::Runtime::Serialization;
				DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
				serializserSettings->PreserveObjectReferences = true;
				List<Type^>^ knownTypes = gcnew List<Type^>();
				knownTypes->AddRange(System::Reflection::Assembly::GetAssembly(Resource::typeid)->ExportedTypes);
				knownTypes->AddRange(System::Reflection::Assembly::GetAssembly(Color::typeid)->ExportedTypes);
				serializserSettings->KnownTypes = knownTypes;
				DataContractSerializer^ serializer = gcnew DataContractSerializer(resource->GetType(), serializserSettings);
				System::IO::FileInfo^ fi = gcnew System::IO::FileInfo(resource->m_path);

				fi->Directory->Create();
				Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
				settings->Indent = true;
				Xml::XmlWriter^ file = Xml::XmlWriter::Create(resource->m_path, settings);
				serializer->WriteObject(file, resource);
				file->Close();
				Monitor::Exit(resourceLock);
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
				else if (extension == "wav")
				{
					return AssetTypes::AUDIO_CLIP;
				}
				else if (extension == "obj" || extension == "fbx")
				{
					return AssetTypes::MODEL;
				}
				else if (extension == "anim")
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
				else if (extension == "prefab")
					return AssetTypes::PREFAB;
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
						case AssetTypes::ANIMATION:
							obj = gcnew Animation(path);
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
				{
					resources[resource]->~Resource();
				}
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
			generic<typename T>
				where T : Resource
			List<T>^ Resources::GetResourcesOfType()
			{
				return (List<T>^)Enumerable::OfType<T>(resources->Values);
			}
#pragma endregion

}