
#include "Model.h"
#include "AudioClip.h"
#include "Material.h"
#include "Shader.h"
#include "Animation.h"
#include "texture\Texture2D.h"
#include "Resources.h"
#include "../Scene.h"
namespace ThomasEngine
{
	Resource^ Resources::Load(String^ path)
	{
		String^ fullPath = System::IO::Path::GetFullPath(path);
		if (resources->ContainsKey(fullPath))
		{
			Resource^ obj = resources[fullPath];
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
				resources[fullPath] = obj;
			}
			return obj;
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
	Resource ^ Resources::Find(String ^ path)
	{
		if (resources->ContainsKey(System::IO::Path::GetFullPath(path)))
		{
			return resources[System::IO::Path::GetFullPath(path)];
		}
		return nullptr;
	}
	void Resources::RenameResource(String ^ oldPath, String ^ newPath)
	{
		if (resources->ContainsKey(System::IO::Path::GetFullPath(oldPath)))
		{
			Object^ lock = Scene::CurrentScene->GetGameObjectsLock();

			System::Threading::Monitor::Enter(lock);
			Resource^ resource = resources[System::IO::Path::GetFullPath(oldPath)];
			resources->Remove(System::IO::Path::GetFullPath(oldPath));
			resources[System::IO::Path::GetFullPath(newPath)] = resource;
			if (resource)
				resource->Rename(newPath);

			System::Threading::Monitor::Exit(lock);
		}
	}


	bool Resources::CreateResource(Resource^ resource, String^ path)
	{
		path = Application::currentProject->assetPath + "\\" + path;
		String^ extension = IO::Path::GetExtension(path);
		String^ modifier = "";
		path = path->Remove(path->Length - extension->Length, extension->Length);
		int i = 0;
		while (IO::File::Exists(path + modifier + extension))
		{
			i++;
			modifier = "(" + i + ")";
		}
		path = path + modifier + extension;
		Monitor::Enter(resourceLock);
		using namespace System::Runtime::Serialization;

		resource->Rename(path);	// Set file name

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
				serializserSettings->KnownTypes = System::Reflection::Assembly::GetAssembly(Resource::typeid)->ExportedTypes;
				// Write File
				DataContractSerializer^ serializer = gcnew DataContractSerializer(resource->GetType(), serializserSettings);
				serializer->WriteObject(file, resource);
				// Success: Append resource
				resources[System::IO::Path::GetFullPath(path)] = resource;
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
}