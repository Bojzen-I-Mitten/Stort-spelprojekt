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
			serializer->PreserveReferencesHandling = PreserveReferencesHandling::All;
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
	}

	void Serializer::SerializeScene(Scene ^ scene, System::String^ path)
	{
		StringWriter^ writer;
		try
		{
			rootType = Scene::typeid;
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
			rootType = Scene::typeid;
			WaitForFile(path, 10);
			StreamReader^ file = File::OpenText(path);
			return (Scene^)serializer->Deserialize(file, Scene::typeid);
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
			rootType = GameObject::typeid;
			writer = gcnew StringWriter();
			serializer->Serialize(writer, gameObject);
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

	System::IO::Stream ^ Serializer::SerializeGameObject(GameObject ^ gameObject)
	{
		try
		{
			rootType = GameObject::typeid;
			System::IO::Stream^ stream = gcnew System::IO::MemoryStream();
			serializer->Serialize(gcnew StreamWriter(stream), gameObject);
			return stream;
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
			rootType = GameObject::typeid;
			WaitForFile(path, 10);
			return DeserializeGameObject(File::Open(path, FileMode::Open));
		}
		catch (Exception^ e)
		{
			Debug::LogError("Failed to deserialize gameObject: " + path + " Error: " + e->Message);
			return nullptr;
		}
	}

	GameObject ^ Serializer::DeserializeGameObject(System::IO::Stream ^ stream)
	{
		try {
			rootType = GameObject::typeid;
			return (GameObject^)serializer->Deserialize(gcnew StreamReader(stream), GameObject::typeid);
			stream->Close();
		}
		catch (Exception^ e)
		{
			Debug::LogError("Failed to deserialize gameObject" + " Error: " + e->Message);
			stream->Close();
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
			return (Material^)serializer->Deserialize(file, Material::typeid);
		}
		catch (Exception^ e) {
			Debug::LogError("Failed to deserialize material: " + path + " Error: " + e->Message);
			return nullptr;
		}
	}

}
