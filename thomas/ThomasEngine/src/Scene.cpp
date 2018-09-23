#include "Scene.h"
#include "object\GameObject.h"
#include "object\Component.h"
#include "resource\Resources.h"
#include "ThomasManaged.h"
#include "Debug.h"
#include <thomas/editor/EditorCamera.h>

namespace ThomasEngine
{
	Scene::Scene(System::String^ name) 
	:	m_gameObjects(gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>()) {
		m_name = name;;
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}
	Scene::Scene()
		: m_gameObjects(gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>()) {
		m_name = "New Scene";
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
	}

	void Scene::Play()
	{
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		savingEnabled = false;
		SaveSceneAs(this, tempFile);
		m_playing = true;
		//for each(GameObject^ gObj in m_gameObjects)
		//{
		//	gObj->Awake();
		//}
	}

	Scene^ Scene::CurrentScene::get()
	{
		return s_currentScene;
	}

	void Scene::CurrentScene::set(Scene^ value)
	{
		if(value == nullptr)	// If clear scene
			value = gcnew Scene("test");
		ThomasWrapper::Selection->UnselectGameObjects();
		s_currentScene = value;
		if(Application::currentProject && savingEnabled)
			Application::currentProject->currentScenePath = value->m_relativeSavePath;

		OnCurrentSceneChanged(value);
	}


	void Scene::SaveSceneAs(Scene ^ scene, System::String ^ path)
	{
		try {
			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();

			auto list = Component::GetAllComponentTypes();
			list->Add(SceneResource::typeid);
			serializserSettings->KnownTypes = list->ToArray();
			serializserSettings->PreserveObjectReferences = true;
			serializserSettings->DataContractSurrogate = gcnew SceneSurrogate();
			DataContractSerializer^ serializer = gcnew DataContractSerializer(Scene::typeid, serializserSettings);
			System::IO::FileInfo^ fi = gcnew System::IO::FileInfo(path);
			fi->Directory->Create();
			Xml::XmlWriterSettings^ settings = gcnew Xml::XmlWriterSettings();
			settings->Indent = true;
			Xml::XmlWriter^ file = Xml::XmlWriter::Create(path, settings);
			serializer->WriteObject(file, scene);
			file->Close();

			if (Application::currentProject && scene->RelativeSavePath != path && savingEnabled) {
				scene->m_relativeSavePath = path->Replace(Application::currentProject->assetPath + "\\", "");
				Application::currentProject->currentScenePath = scene->RelativeSavePath;
			}
		}
		catch (Exception^ e) {
			Debug::Log("Failed to save scene: Error: " + e->Message);
		}

			
	}

	void Scene::SaveScene(Scene ^ scene)
	{
		if(Application::currentProject && scene->RelativeSavePath)
			SaveSceneAs(scene, Application::currentProject->assetPath + "\\" + scene->RelativeSavePath);
	}

	Scene ^ Scene::LoadScene(System::String ^ fullPath)
	{
		if (!File::Exists(fullPath))
		{
			Debug::Log("Unable to find scene: " + fullPath);
			return nullptr;
		}
		Scene^ scene;
		try {
			s_loading = true;

			using namespace System::Runtime::Serialization;
			DataContractSerializerSettings^ serializserSettings = gcnew DataContractSerializerSettings();
			auto list = Component::GetAllComponentTypes();
			list->Add(SceneResource::typeid);
			serializserSettings->KnownTypes = list;
			serializserSettings->PreserveObjectReferences = true;
			serializserSettings->DataContractSurrogate = gcnew SceneSurrogate();
			DataContractSerializer^ serializer = gcnew DataContractSerializer(Scene::typeid, serializserSettings);
			Xml::XmlReader^ file = Xml::XmlReader::Create(fullPath);
			scene = (Scene^)serializer->ReadObject(file);

			scene->EnsureLoad();

			msclr::interop::marshal_context context;
			for (int i = 0; i < scene->GameObjects->Count; ++i)
				scene->GameObjects[i]->nativePtr->SetName(context.marshal_as<std::string>(scene->GameObjects[i]->Name));

			file->Close();

			scene->PostLoad();
			if (Application::currentProject && savingEnabled)
				scene->m_relativeSavePath = fullPath->Replace(Application::currentProject->assetPath + "\\", "");
			return scene;
		}
		catch (Exception^ e) {
			LOG("Loading scene: ");
			LOG(Utility::ConvertString(fullPath));
			LOG("with error:")
			LOG(Utility::ConvertString(e->Message));
			scene = nullptr;
		}
		finally{
			s_loading = false;
		}
		return scene;
	}

	void Scene::RestartCurrentScene()
	{
		Object^ lock = Scene::CurrentScene->GetGameObjectsLock();
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		Monitor::Enter(lock);
		Scene::CurrentScene->UnLoad();
		Scene::CurrentScene = Scene::LoadScene(tempFile);
		try {
			System::IO::File::Delete(tempFile);
		}
		catch (Exception^ E)
		{

		}
		Monitor::Exit(lock);
		savingEnabled = true;
	}

	void Scene::UnLoad()
	{
		for (int i = 0; i < m_gameObjects->Count; i++)
		{
			m_gameObjects[i]->Destroy();
			i--;
		}
		m_gameObjects->Clear();

	}

	void Scene::EnsureLoad()
	{

		if (m_gameObjects == nullptr) { // Scene is empty
			LOG("Warning, no objects in scene.");
			m_gameObjectsLock = gcnew Object();
			m_gameObjects = gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>();
			System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
		}

	}

	void Scene::PostLoad()
	{
		for each(GameObject^ gObj in m_gameObjects)
		{
			gObj->PostLoad(this);
		}
	}

	System::Type ^ Scene::SceneSurrogate::GetDataContractType(System::Type ^ type)
	{
		if (type->BaseType == Resource::typeid)
		{
			return SceneResource::typeid;
		}
		else
		{
			return type;
		}
	}

	System::Object ^ Scene::SceneSurrogate::GetObjectToSerialize(System::Object ^obj, System::Type ^targetType)
	{
		if (obj->GetType()->BaseType == Resource::typeid)
		{
			Resource^ resource = (Resource^)obj;
			return gcnew SceneResource(resource->GetAssetRelativePath());
		}
		else if (obj->GetType() == GameObject::typeid) {
			GameObject^ gameObject = (GameObject^)obj;
			if(gameObject->prefabPath)
				return gcnew SceneResource(Resources::ConvertToThomasPath(gameObject->prefabPath));
		}
		else if (Component::typeid->IsAssignableFrom(obj->GetType())) {
			Component^ component = (Component^)obj;
			if (component->gameObject->prefabPath)
				return gcnew SceneResource(Resources::ConvertToThomasPath(component->gameObject->prefabPath));
		}
		return obj;
	}

	System::Object ^ Scene::SceneSurrogate::GetDeserializedObject(System::Object ^obj, System::Type ^targetType)
	{
		if (obj->GetType() == SceneResource::typeid)
		{
			SceneResource^ sceneResource = (SceneResource^)obj;
			if (sceneResource->path == "")
			{
				if (targetType == Material::typeid)
					return Material::StandardMaterial;
			}
			else if (targetType == GameObject::typeid)
				return Resources::LoadPrefab(Resources::ConvertToRealPath(sceneResource->path));
			else if (Component::typeid->IsAssignableFrom(targetType)) {
				return Resources::LoadPrefab(Resources::ConvertToRealPath(sceneResource->path))->GetComponent(targetType);
			}
			else {
				return Resources::LoadThomasPath(sceneResource->path);
			}
		}
		return obj;
	}

	System::Object ^ Scene::SceneSurrogate::GetCustomDataToExport(System::Reflection::MemberInfo ^memberInfo, System::Type ^dataContractType)
	{
		throw gcnew NotSupportedException("unused");
	}

	System::Object ^ Scene::SceneSurrogate::GetCustomDataToExport(System::Type ^clrType, System::Type ^dataContractType)
	{
		throw gcnew NotSupportedException("unused");
	}

	void Scene::SceneSurrogate::GetKnownCustomDataTypes(System::Collections::ObjectModel::Collection<System::Type ^> ^customDataTypes)
	{
		throw gcnew NotSupportedException("unused");
	}

	System::Type ^ Scene::SceneSurrogate::GetReferencedTypeOnImport(System::String ^typeName, System::String ^typeNamespace, System::Object ^customData)
	{
		throw gcnew NotSupportedException("unused");
	}

	System::CodeDom::CodeTypeDeclaration ^ Scene::SceneSurrogate::ProcessImportedType(System::CodeDom::CodeTypeDeclaration ^typeDeclaration, System::CodeDom::CodeCompileUnit ^compileUnit)
	{
		throw gcnew NotSupportedException("unused");
	}

	Vector3 Scene::CameraPosition::get() {
		return Utility::Convert(thomas::editor::EditorCamera::GetEditorCamera()->m_transform->GetLocalPosition());
	}

	void Scene::CameraPosition::set(Vector3 pos) {

		thomas::editor::EditorCamera::GetEditorCamera()->m_transform->SetLocalPosition(Utility::Convert(pos));
	}

	Vector3 Scene::CameraEuler::get() {
		return Utility::Convert(thomas::editor::EditorCamera::GetEditorCamera()->m_transform->GetLocalEulerAngles());
	}

	void Scene::CameraEuler::set(Vector3 euler) {

		thomas::editor::EditorCamera::GetEditorCamera()->m_transform->SetLocalRotation(euler.y, euler.x, euler.z);
	}

}

