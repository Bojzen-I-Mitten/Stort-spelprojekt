
#include "GameObject.h"
#include "..\ThomasManaged.h"

#include "component\physics\BoxCollider.h"
#include "component\physics\SphereCollider.h"
#include "..\Debug.h"
#include "../Utility.h"
namespace ThomasEngine {

	void GameObject::Destroy()
	{
		if (m_isDestroyed)
			return;
		m_isDestroyed = true;
		Monitor::Enter(this->scene->CurrentScene->GetGameObjectsLock());
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++) {
			m_components[i]->Destroy();
			i--;
		}
		Object::Destroy();
		m_components.Clear();
		Monitor::Exit(m_componentsLock);
		ThomasWrapper::Selection->UnSelectGameObject(this);
		this->scene->GameObjects->Remove(this);
		Monitor::Exit(this->scene->CurrentScene->GetGameObjectsLock());
	}

	GameObject ^ ThomasEngine::GameObject::CreatePrimitive(PrimitiveType type)
	{
		GameObject^ gameObject = gcnew GameObject("new" + type.ToString());
		gameObject->AddComponent<RenderComponent^>()->model = Model::GetPrimitive(type);
		return gameObject;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original)
	{
		if(!original){
			Debug::Log("Object to instantiate is null");
			return nullptr;
		}
		System::IO::Stream^ serialized = SerializeGameObject(original);

		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
		GameObject^ clone = DeSerializeGameObject(serialized);
		if(clone)
			clone->PostInstantiate(Scene::CurrentScene);
		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Transform^ parent)
	{
		GameObject^ clone = Instantiate(original);
		if(clone)
			clone->transform->parent = parent;
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Vector3 position, Quaternion rotation)
	{
		GameObject^ clone = Instantiate(original);
		if(clone){
			clone->transform->position = position;
			clone->transform->rotation = rotation;
		}
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Vector3 position, Quaternion rotation, Transform^ parent)
	{
		GameObject^ clone = Instantiate(original, parent);
		if (clone) {
			clone->transform->position = position;
			clone->transform->rotation = rotation;
		}
		return clone;
	}



	void GameObject::SerializeGameObject(String ^ path, GameObject ^ gObj)
	{
		using namespace System::Runtime::Serialization;
		DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
		auto list = Component::GetAllComponentTypes();
		list->Add(SceneResource::typeid);
		serializerSettings->KnownTypes = list;
		serializerSettings->PreserveObjectReferences = true;
		serializerSettings->DataContractSurrogate = gcnew Scene::SceneSurrogate();
		DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

		Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
		settings->Indent = true;
				
		Xml::XmlWriter^ writer = Xml::XmlWriter::Create(path, settings);
		serializer->WriteObject(writer, gObj);
		writer->Close();
	}

	System::IO::Stream^ GameObject::SerializeGameObject(GameObject ^ gObj)
	{
		using namespace System::Runtime::Serialization;
		DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
		auto list = Component::GetAllComponentTypes();
		list->Add(SceneResource::typeid);
		serializerSettings->KnownTypes = list;
		serializerSettings->PreserveObjectReferences = true;
		serializerSettings->DataContractSurrogate = gcnew Scene::SceneSurrogate();
		DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

		System::IO::Stream^ stream = gcnew System::IO::MemoryStream();
		serializer->WriteObject(stream, gObj);
		return stream;
	}

	GameObject ^ GameObject::DeSerializeGameObject(System::IO::Stream ^ stream)
	{
		try {
			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
			auto list = Component::GetAllComponentTypes();
			list->Add(SceneResource::typeid);
			serializerSettings->KnownTypes = list;
			serializerSettings->PreserveObjectReferences = true;
			serializerSettings->DataContractSurrogate = gcnew Scene::SceneSurrogate();
			DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

			stream->Seek(0, SeekOrigin::Begin);
			GameObject^ gObj = (GameObject^)serializer->ReadObject(stream);
			gObj->PostLoad(nullptr);
			return gObj;
		}
		catch (Exception^ e) {
			std::string msg("Failed to load gameObject, msg: " + Utility::ConvertString(e->Message));
			LOG(msg);
			return nullptr;
		}
		

	}

}