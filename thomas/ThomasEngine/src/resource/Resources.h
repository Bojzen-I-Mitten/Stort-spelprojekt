#pragma once

namespace thomas { namespace resource { class Resource; } }
using namespace System;
namespace ThomasEngine
{
	ref class GameObject;
	ref class Resource;
	public ref class Resources
	{
	private:
		static Object^ resourceLock = gcnew Object();
		static System::Collections::Generic::Dictionary<String^, Resource^>^ resources = gcnew System::Collections::Generic::Dictionary<String^, ThomasEngine::Resource^>();
	internal:

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
			FONT,
			UNKNOWN
		};

		delegate void ResourceDelegate();
		static event ResourceDelegate^ OnResourceLoadStarted;
		static event ResourceDelegate^ OnResourceLoadEnded;

		delegate void ResourceLoad(String^ name, int index, int total);
		static event ResourceLoad^ OnResourceLoad;


		static void OnPlay();
		static void OnStop();
		static void SavePrefab(GameObject ^ gameObject, String ^ path);
		static GameObject ^ LoadPrefab(String^ path);
		static GameObject ^ LoadPrefab(String^ path, bool forceInstantiate);


		static String^ GetUniqueName(String^ path);

		static bool CreateResource(Resource^ resource, String^ path);

		static AssetTypes GetResourceAssetType(Type^ type);

		static AssetTypes GetResourceAssetType(String^ path);
		static bool IsResource(String^ path);

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
		/* Load resource using converted file path to thomas internal representation
		*/
		static Resource ^ LoadThomasPath(String ^ path);

		/* Load resource using absolute/relative file path
		*/
		static Resource ^ LoadSysPath(String ^ system_path);
		
		static Resource^ Find(String^ path);

		static Resource^ LoadErrorResource(AssetTypes type);

		static void RenameResource(String^ oldPath, String^ newPath);


		static void Unload(Resource^ resource);

		static void LoadAll(String^ path);

		static void UnloadAll();

	private:
		static	Resource ^ Load(String^ sysPath, String^ thomasPath);
	};
}