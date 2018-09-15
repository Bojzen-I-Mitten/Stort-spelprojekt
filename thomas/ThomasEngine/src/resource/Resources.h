#pragma once

namespace thomas { namespace resource { class Resource; } }
namespace ThomasEngine
{
	ref class GameObject;
	ref class Resource;
	public ref class Resources
	{
	internal:
		static Object^ resourceLock = gcnew Object();
		static System::Collections::Generic::Dictionary<String^, Resource^>^ resources = gcnew System::Collections::Generic::Dictionary<String^, ThomasEngine::Resource^>();

		generic<typename T>
		where T : Resource
		static T Deserialize(String^ path);
	public:
		enum class AssetTypes
		{
			MODEL,
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

		static void OnPlay();
		static void OnStop();

		static void Resources::SavePrefab(GameObject ^ gameObject, String ^ path);
		static GameObject ^ Resources::LoadPrefab(String^ path);

		static void SaveResource(Resource^ resource);

		static String^ GetUniqueName(String^ path);

		static void CreateResource(Resource^ resource, String^ path);

		static AssetTypes GetResourceAssetType(Type^ type);

		static AssetTypes GetResourceAssetType(String^ path);

		static String^ ConvertToThomasPath(String^ value);
		static String^ ConvertToRealPath(String^ value);

		generic<typename T>
		where T : Resource
		static T Load(String^ path);

		generic<typename T>
		where T : Resource
		static System::Collections::Generic::List<T>^ GetResourcesOfType();

		static System::Collections::Generic::List<Resource^>^ GetResourcesOfType(Type^ type);

		static Resource^ FindResourceFromNativePtr(thomas::resource::Resource* nativePtr);

		static Resource^ Load(String^ path);
		
		static Resource^ Find(String^ path);

		static void RenameResource(String^ oldPath, String^ newPath);


		static void Unload(Resource^ resource);

		static void LoadAll(String^ path);

		static void UnloadAll();
	};
}