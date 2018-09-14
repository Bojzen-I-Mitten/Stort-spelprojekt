#pragma once
#pragma unmanaged
#include "thomas\resource\Resource.h"
#pragma managed
#include "Resource.h"
#include "../Application.h"

using namespace System::Collections::Generic;
using namespace System::Linq;
using namespace System::Threading;
namespace ThomasEngine
{
	public ref class Resources
	{
	internal:
		static Object^ resourceLock = gcnew Object();
		static Dictionary<String^, Resource^>^ resources = gcnew Dictionary<String^, ThomasEngine::Resource^>();

		generic<typename T>
		where T : Resource
		static T Deserialize(String^ path)
		{
			
			Monitor::Enter(resourceLock);
			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
			serializserSettings->PreserveObjectReferences = true;
			serializserSettings->KnownTypes = System::Reflection::Assembly::GetAssembly(Resource::typeid)->ExportedTypes;
			DataContractSerializer^ serializer = gcnew DataContractSerializer(T::typeid, serializserSettings);

			Xml::XmlReader^ file = Xml::XmlReader::Create(path);
			T resource = (T)serializer->ReadObject(file);
			file->Close();
			resource->Rename(path);
			Monitor::Exit(resourceLock);
			return resource;
			
			
		}
	public:
		enum class AssetTypes
		{
			MODEL,
			ANIMATION,
			TEXTURE2D,
			TEXTURE3D,
			SCENE,
			SHADER,
			MATERIAL,
			SCRIPT,
			AUDIO_CLIP,
			PREFAB,
			UNKNOWN
		};

		static void OnPlay()
		{
			for each (Resource^ resource in resources->Values)
			{
				resource->OnPlay();
			}
		}
		static void OnStop()
		{
			for each (Resource^ resource in resources->Values)
			{
				resource->OnStop();
			}
		}

		static void Resources::SavePrefab(GameObject ^ gameObject, String ^ path);
		static GameObject ^ Resources::LoadPrefab(String^ path);

		static void SaveResource(Resource^ resource)
		{
			
			Monitor::Enter(resourceLock);
			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
			serializserSettings->PreserveObjectReferences = true;
			serializserSettings->KnownTypes = System::Reflection::Assembly::GetAssembly(Resource::typeid)->ExportedTypes;
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

		static String^ GetUniqueName(String^ path) {
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

		static void CreateResource(Resource^ resource, String^ path)
		{
			path = GetUniqueName(Application::currentProject->assetPath + "\\" + path);
			Monitor::Enter(resourceLock);
			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
			serializserSettings->PreserveObjectReferences = true;
			serializserSettings->KnownTypes = System::Reflection::Assembly::GetAssembly(Resource::typeid)->ExportedTypes;
			DataContractSerializer^ serializer = gcnew DataContractSerializer(resource->GetType(), serializserSettings);
			System::IO::FileInfo^ fi = gcnew System::IO::FileInfo(path);

			fi->Directory->Create();
			Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
			settings->Indent = true;
			Xml::XmlWriter^ file = Xml::XmlWriter::Create(path, settings);
			resource->Rename(path);
			serializer->WriteObject(file, resource);
			file->Close();
			resources[System::IO::Path::GetFullPath(path)] = resource;
			Monitor::Exit(resourceLock);
		}
		static bool CreateResource(Resource^ resource, String^ path);

		static AssetTypes GetResourceAssetType(Type^ type);

		static AssetTypes GetResourceAssetType(String^ path)
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

		static String^ ConvertToThomasPath(String^ value) {
			value = System::IO::Path::GetFullPath(value);
			if (value->Contains(Application::editorAssets)) return value->Replace(Application::editorAssets, "%THOMAS_DATA%");
			else return value->Replace(Application::currentProject->assetPath, "%THOMAS_ASSETS%");
		}
		static String^ ConvertToRealPath(String^ value) {
			value = value->Replace("%THOMAS_DATA%", Application::editorAssets);
			value = value->Replace("%THOMAS_ASSETS%", Application::currentProject->assetPath);
			return value;
		}

		generic<typename T>
		where T : Resource
		static T Load(String^ path) 
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

		generic<typename T>
		where T : Resource
		static List<T>^ GetResourcesOfType()
		{
			return (List<T>^)Enumerable::OfType<T>(resources->Values);
		}

		static List<Resource^>^ GetResourcesOfType(Type^ type)
		{
			List<Resource^>^ list = gcnew List<Resource^>();
			for each (Resource^ resource in resources->Values)
			{
				if (resource->GetType() == type)
					list->Add(resource);
			}
			return list;
		}

		static Resource^ FindResourceFromNativePtr(thomas::resource::Resource* nativePtr)
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

		static Resource^ Load(String^ path);
		
		static Resource^ Find(String^ path);

		static void RenameResource(String^ oldPath, String^ newPath);


		static void Unload(Resource^ resource) {
			if (Find(resource->m_path))
			{
				resources->Remove(System::IO::Path::GetFullPath(resource->m_path));
			}
		}

		static void LoadAll(String^ path)
		{
			array<String^>^ directories = IO::Directory::GetDirectories(path);
			array<String^>^ files = IO::Directory::GetFiles(path);
			for each(String^ dir in directories)
			{
				LoadAll(dir);
			}
			for each(String^ file in files)
			{
				Load(file);
			}
		}

		static void UnloadAll()
		{
			for each(String^ resource in resources->Keys)
			{
				resources[resource]->~Resource();
			}
		}
	};
}