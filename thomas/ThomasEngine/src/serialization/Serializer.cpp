#include "Serializer.h"
#include "Converters.h"
#include "../object/GameObject.h"
#include "../resource/Material.h"
#include "../Debug.h"


using namespace System::IO;
namespace ThomasEngine
{

	JsonSerializer^ Serializer::CreateSerializerBase()
	{
		JsonSerializer^ serializer = gcnew JsonSerializer();

		serializer->Formatting = Formatting::Indented;
		serializer->ReferenceLoopHandling = ReferenceLoopHandling::Ignore;									// Serialize recursive $ref $id dependencies	
		serializer->PreserveReferencesHandling = PreserveReferencesHandling::Objects;						// Give $ref $id to objects (not lists)
		serializer->ConstructorHandling = ConstructorHandling::AllowNonPublicDefaultConstructor;
		serializer->TypeNameHandling = TypeNameHandling::Auto;
		serializer->NullValueHandling = NullValueHandling::Ignore;
		serializer->ObjectCreationHandling = ObjectCreationHandling::Auto;									// Specifies if objects are created in-place or not
		serializer->MetadataPropertyHandling = MetadataPropertyHandling::Default;							// Specifies if $ref $id handling reads forward before resolving conflict?
		serializer->DefaultValueHandling = DefaultValueHandling::Populate;									// Set to default value
		serializer->Error += gcnew EventHandler<Serialization::ErrorEventArgs^>(&Serializer::ErrorHandler);
		return serializer;
	}
	
	JsonSerializer^ Serializer::serializer::get()
	{
		JsonSerializer^ serializer = CreateSerializerBase();

		serializer->Converters->Add(gcnew ResourceConverter());
		serializer->Converters->Add(gcnew PrefabConverter());
		serializer->Converters->Add(gcnew ComponentConverter());
		return serializer;
	}
	JsonSerializer^ Serializer::SerializerMaterial::get()
	{
		JsonSerializer^ serializer = CreateSerializerBase();

		serializer->Converters->Add(gcnew ResourceConverter(Material::typeid));
		serializer->Converters->Add(gcnew PrefabConverter());
		serializer->Converters->Add(gcnew ComponentConverter());
		return serializer;

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

	JArray^ Serializer::SerializeGameObject(GameObject ^ gameObject)
	{
		try
		{
			List<GameObject^>^ flatGameObjects = gcnew List<GameObject^>();
			GameObject::FlattenGameObjectTree(flatGameObjects, gameObject);
			JArray^ ja = JArray::FromObject(flatGameObjects, serializer);
			return ja;
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

	GameObject^ Serializer::DeserializeGameObject(Newtonsoft::Json::Linq::JArray^ ja)
	{
		try {

			List<GameObject^>^ gameObject = ja->ToObject<List<GameObject^>^>(serializer);
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
			writer = gcnew StringWriter();
			SerializerMaterial->Serialize(writer, material);
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
			WaitForFile(path, 10);
			StreamReader^ file = File::OpenText(path);
			Material^ material = (Material^)SerializerMaterial->Deserialize(file, Material::typeid);
			material->Path = path;
			file->Close();
			return material;
		}
		catch (Exception^ e) {
			Debug::LogError("Failed to deserialize material: " + path + " Error: " + e->Message);
			return nullptr;
		}
	}

	void Serializer::ErrorHandler(Object ^ sender, Serialization::ErrorEventArgs ^ args)
	{
		Debug::LogException(args->ErrorContext->Error);
		args->ErrorContext->Handled = true;
	}

}
