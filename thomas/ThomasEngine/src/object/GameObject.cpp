
#include "GameObject.h"
#include "..\ThomasManaged.h"

#include "component\physics\BoxCollider.h"
#include "component\physics\SphereCollider.h"

namespace ThomasEngine {

	void GameObject::Destroy()
	{
		if (m_isDestroyed)
			return;
		m_isDestroyed = true;
		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
		Monitor::Enter(m_componentsLock);
		for (int i = 0; i < m_components.Count; i++) {
			m_components[i]->Destroy();
			i--;
		}
		Object::Destroy();
		m_components.Clear();
		Monitor::Exit(m_componentsLock);
		ThomasWrapper::Selection->UnSelectGameObject(this);
		Scene::CurrentScene->GameObjects->Remove(this);
		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
	}

	GameObject ^ ThomasEngine::GameObject::CreatePrimitive(PrimitiveType type)
	{
		GameObject^ gameObject = gcnew GameObject("new" + type.ToString());
		gameObject->AddComponent<RenderComponent^>()->model = Model::GetPrimitive(type);
		return gameObject;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original)
	{
		System::IO::Stream^ serialized = SerializeGameObject(original);

		Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
		GameObject^ clone = DeSerializeGameObject(serialized);
		clone->scene = Scene::CurrentScene;
		Scene::CurrentScene->GameObjects->Add(clone);
		
		Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Transform^ parent)
	{
		GameObject^ clone = Instantiate(original);
		clone->transform->parent = parent;
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Vector3 position, Quaternion rotation)
	{
		GameObject^ clone = Instantiate(original);
		clone->transform->position = position;
		clone->transform->rotation = rotation;
		return clone;
	}

	GameObject ^ GameObject::Instantiate(GameObject ^ original, Vector3 position, Quaternion rotation, Transform^ parent)
	{
		GameObject^ clone = Instantiate(original, parent);
		clone->transform->position = position;
		clone->transform->rotation = rotation;
		return clone;
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

		Xml::XmlWriter^ writer = Xml::XmlWriter::Create(stream);

		serializer->WriteObject(writer, gObj);

		Xml::XmlReader^ file = Xml::XmlReader::Create(stream);
		return stream;
	}

	GameObject ^ GameObject::DeSerializeGameObject(System::IO::Stream ^ stream)
	{
		using namespace System::Runtime::Serialization;
		DataContractSerializerSettings^ serializerSettings = gcnew DataContractSerializerSettings();
		auto list = Component::GetAllComponentTypes();
		list->Add(SceneResource::typeid);
		serializerSettings->KnownTypes = list;
		serializerSettings->PreserveObjectReferences = true;
		serializerSettings->DataContractSurrogate = gcnew Scene::SceneSurrogate();
		DataContractSerializer^ serializer = gcnew DataContractSerializer(GameObject::typeid, serializerSettings);

		Xml::XmlReader^ reader = Xml::XmlReader::Create(stream);

		GameObject^ gObj = (GameObject^)serializer->ReadObject(reader);
		return gObj;

	}

}