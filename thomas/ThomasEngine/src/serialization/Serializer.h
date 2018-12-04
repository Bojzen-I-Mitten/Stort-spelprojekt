#pragma once

using namespace Newtonsoft::Json;
namespace ThomasEngine
{
	ref class Scene;
	ref class GameObject;
	ref class Material;
	public ref class Serializer
	{
	private:
		/* Create a serializer with base properties set
		*/
		static JsonSerializer^ CreateSerializerBase();
	internal:
		static property JsonSerializer^ serializer {
			JsonSerializer^ get();
		}
		/* Create a serializer specified for serializing/deserializing materials
		*/
		static property JsonSerializer^ SerializerMaterial {
			JsonSerializer^ get();
		}

		static void SerializeScene(Scene^ scene, System::String^ path);
		static Scene^ DeserializeScene(System::String^ path);

		static void SerializeGameObject(GameObject^ gameObject, System::String^ path);
		static Newtonsoft::Json::Linq::JArray^ SerializeGameObject(GameObject^ gameObject);

		static GameObject^ DeserializeGameObject(System::String^ path);
		static GameObject^ DeserializeGameObject(Newtonsoft::Json::Linq::JArray^ ja);

		static void SerializeMaterial(Material^ material, System::String^ path);
		static Material^ DeserializeMaterial(System::String^ path);

		static void ErrorHandler(Object^ sender, Serialization::ErrorEventArgs^ args);

	public:
		static bool IsFileReady(System::String^ path);
		//Tries to ready a file with 0.1s delay between retries
		static void WaitForFile(System::String^ path, int numberOfRetries);
	};
}