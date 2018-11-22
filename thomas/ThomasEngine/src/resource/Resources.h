#pragma once

namespace thomas { namespace resource { class Resource; } }
using namespace System;
using namespace System::Collections::Generic;
namespace ThomasEngine
{
	ref class GameObject;
	ref class Resource;
	public ref class Resources
	{
	private:
		ref class AssetLoadWorker
		{
		private:
			String^ file; 
			System::Threading::CountdownEvent^ countdown;

		public:
			AssetLoadWorker(String^ file, System::Threading::CountdownEvent^ countdown)
			{
				this->file = file;
				this->countdown = countdown;
			}
			void LoadAsset(System::Object ^state);
		};
		static Object^ resourceLock = gcnew Object();
		static Dictionary<String^, Resource^>^ resources = gcnew Dictionary<String^, Resource^>();
		static Dictionary<String^, GameObject^>^ s_PREFAB_DICT = gcnew Dictionary<String^, GameObject^>();
		/* Load a list of asset using assetworker*/
		static void LoadAssetFiles(List<String^>^ files);
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
		/* Create a new prefab, to be used in a scene context.
		*/
		static GameObject ^ CreatePrefab(String^ path);
		/* Load a prefab as an editor resource, and not to be used.
		*/
		static GameObject ^ LoadPrefabResource(String^ path);
		
#ifdef _EDITOR
		/* Synchronous access to prefab list
		*/
		static property IEnumerable<GameObject^>^ PrefabList
		{
			IEnumerable<GameObject^>^ get();
		}
#endif


		static String^ GetUniqueName(String^ path);

		static bool CreateResource(Resource^ resource, String^ path);

		static AssetTypes GetResourceAssetType(Type^ type);

		static AssetTypes GetResourceAssetType(String^ path);
		static bool IsResource(String^ path);

		static bool IsResource(AssetTypes type);

		static String^ ConvertToThomasPath(String^ value);
		static String^ ConvertToRealPath(String^ value);

		generic<typename T>
		where T : Resource
		static T Load(String^ path);

		generic<typename T>
		where T : Resource
		static List<T>^ GetResourcesOfType();

		static List<Resource^>^ GetResourcesOfType(Type^ type);

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