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


		static List<Type^>^ getKnownTypes();

		generic<typename T>
		where T : Resource
		static T Deserialize(String^ path);
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

		static void OnPlay();

		static void OnStop();

		static void SavePrefab(GameObject ^ gameObject, String ^ path);
		static GameObject ^ Resources::LoadPrefab(String^ path);

		static void SaveResource(Resource^ resource);

		static String^ GetUniqueName(String^ path);

		static bool CreateResource(Resource^ resource, String^ path);

		static AssetTypes GetResourceAssetType(Type^ type);

		static AssetTypes GetResourceAssetType(String^ path);

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

		static void RenameResource(String^ oldPath, String^ newPath);


		static void Unload(Resource^ resource);

		static void LoadAll(String^ path);

		static void UnloadAll();

	private:
		static	Resource ^ Load(String^ sysPath, String^ thomasPath);
	};
}