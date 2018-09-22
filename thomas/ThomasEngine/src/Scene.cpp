#include "Scene.h"
#include "ThomasSelection.h"
#include "object\GameObject.h"
#include "object\Component.h"
#include "resource\Resources.h"
#include "SceneSurrogate.h"
#include "Debug.h"
#include "Application.h"
#include "Project.h"
#using "PresentationFramework.dll"
namespace ThomasEngine
{

	Scene::Scene() {
		m_name = "New Scene";
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(%m_gameObjects, m_gameObjectsLock);
	}

	Scene::Scene(System::String^ name) {
		m_name = name;
		System::Windows::Data::BindingOperations::EnableCollectionSynchronization(%m_gameObjects, m_gameObjectsLock);
	}

	void Scene::Play()
	{
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		savingEnabled = false;
		SaveSceneAs(this, tempFile);
		m_playing = true;
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
		if (!IO::File::Exists(fullPath))
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

			file->Close();

			scene->PostLoad();
			if (Application::currentProject && savingEnabled)
				scene->m_relativeSavePath = fullPath->Replace(Application::currentProject->assetPath + "\\", "");
			return scene;
		}
		catch (Exception^ e) {
			Debug::Log("Loading scene: ");
			Debug::Log(fullPath);
			Debug::Log("with error:");
			Debug::Log(e->Message);
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
		for (int i = 0; i < m_gameObjects.Count; i++)
		{
			m_gameObjects[i]->Destroy();
			i--;
		}
		m_gameObjects.Clear();

	}

	void Scene::PostLoad()
	{
		for each(GameObject^ gObj in m_gameObjects)
		{
			gObj->PostLoad(this);
		}
	}





}

