#include "Serializer.h"
#include "Converters.h"
#include "../Debug.h"
#include "../object/GameObject.h"

using namespace System::IO;
namespace ThomasEngine
{

	JsonSerializer^ Serializer::serializer::get()
	{
		if (s_serializer)
			return s_serializer;
		else
		{
			JsonSerializer^ serializer = gcnew JsonSerializer();

			serializer->Formatting = Formatting::Indented;
			serializer->ReferenceLoopHandling = ReferenceLoopHandling::Ignore;
			serializer->PreserveReferencesHandling = PreserveReferencesHandling::Objects;
			serializer->ConstructorHandling = ConstructorHandling::AllowNonPublicDefaultConstructor;
			serializer->TypeNameHandling = TypeNameHandling::Auto;

			serializer->Converters->Add(gcnew ResourceConverter());
			serializer->Converters->Add(gcnew PrefabConverter());
			s_serializer = serializer;
			return s_serializer;
		}
		
	}

	bool Serializer::IsFileReady(System::String ^ path)
	{
		try
		{
			FileStream^ inputStream = File::Open(path, FileMode::Open, FileAccess::Read, FileShare::None);
			inputStream->Close();
			return true;
		}
		catch (Exception^)
		{
			return false;
		}
	}

	void Serializer::WaitForFile(System::String ^ path, int numberOfRetries)
	{
		int retries = 0;
		while (!IsFileReady(path) && retries < numberOfRetries)
		{
			Thread::Sleep(100);
			retries++;
		}
		return;
	}

	void Serializer::SerializeScene(Scene ^ scene, System::String^ path)
	{
		StringWriter^ writer;
		try
		{
			writer = gcnew StringWriter();
			serializer->Serialize(writer, scene);
			File::WriteAllText(path, writer->ToString());
			
		}
		catch (System::Exception^ e)
		{
			Debug::LogError("Failed to serialize scene: " + path + " Error: " + e->Message);
		}
		finally{
			writer->Close();
		}
	}

	Scene^ Serializer::DeserializeScene(System::String ^ path)
	{
		try {

			WaitForFile(path, 10);
			StreamReader^ file = File::OpenText(path);
			Scene^ scene = (Scene^)serializer->Deserialize(file, Scene::typeid);
			file->Close();
			return scene;
		}
		catch (Exception^ e) {
			Debug::LogError("Failed to deserialize scene: " + path + " Error: " + e->Message);
			return nullptr;
		}

	}

	void Serializer::SerializeGameObject(GameObject ^ gameObject, System::String ^ path)
	{
		StringWriter^ writer;
		try
		{
			List<GameObject^>^ flatGameObjects = gcnew List<GameObject^>();
			GameObject::FlattenGameObjectTree(flatGameObjects, gameObject);
			writer = gcnew StringWriter();
			serializer->Serialize(writer, flatGameObjects);
			File::WriteAllText(path, writer->ToString());
		}
		catch (System::Exception^ e)
		{
			Debug::LogError("Failed to serialize scene: " + path + " Error: " + e->Message);
		}finally
		{
			writer->Close();
		}
	}

	JObject^ Serializer::SerializeGameObject(GameObject ^ gameObject)
	{
		try
		{
			List<GameObject^>^ flatGameObjects = gcnew List<GameObject^>();
			GameObject::FlattenGameObjectTree(flatGameObjects, gameObject);
			return JObject::FromObject(flatGameObjects, serializer);
		}
		catch (System::Exception^ e)
		{
			Debug::LogError("Failed to serialize gameObject: " + gameObject->Name + " Error: " + e->Message);
		}
		return nullptr;
	}

	GameObject ^ Serializer::DeserializeGameObject(System::String ^ path)
	{
		try {
			WaitForFile(path, 10);
			StreamReader^ file = File::OpenText(path);
			List<GameObject^>^ gameObject = (List<GameObject^>^)serializer->Deserialize(file, List<GameObject^>::typeid);
			file->Close();
			return gameObject[0];
		}
		catch (Exception^ e)
		{
			Debug::LogError("Failed to deserialize gameObject: " + path + " Error: " + e->Message);
			return nullptr;
		}
	}

	GameObject^ Serializer::DeserializeGameObject(Newtonsoft::Json::Linq::JObject^ jo)
	{
		try {

			List<GameObject^>^ gameObject = jo->ToObject<List<GameObject^>^>(serializer);
			return gameObject[0];
		}
		catch (Exception^ e)
		{
			Debug::LogError("Failed to deserialize gameObject" + " Error: " + e->Message);
			return nullptr;
		}
	}

	void Serializer::SerializeMaterial(Material ^ material, System::String ^ path)
	{
		StringWriter^ writer;
		try
		{
			rootType = Material::typeid;
			writer = gcnew StringWriter();
			serializer->Serialize(writer, material);
			rootType = nullptr;
			File::WriteAllText(path, writer->ToString());
		}
		catch (System::Exception^ e)
		{
			Debug::LogError("Failed to serialize material: " + path + " Error: " + e->Message);
		}
		finally
		{
			writer->Close();
		}
	}

	Material ^ Serializer::DeserializeMaterial(System::String ^ path)
	{
		try {
			rootType = Material::typeid;
			WaitForFile(path, 10);
			StreamReader^ file = File::OpenText(path);
			Material^ material = (Material^)serializer->Deserialize(file, Material::typeid);
			rootType = nullptr;
			file->Close();
			return material;
		}
		catch (Exception^ e) {
			Debug::LogError("Failed to deserialize material: " + path + " Error: " + e->Message);
			return nullptr;
		}
	}

}
