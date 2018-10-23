#pragma unmanaged

#include <assert.h>

#pragma managed

#include "SceneManager.h"
#include "../ThomasManaged.h"
#include "../ThomasSelection.h"
#include "../Application.h"
#include "../Project.h"
#include "../Debug.h"

namespace ThomasEngine
{

	SceneManager::SceneManager()
		: 
		m_ID_Counter(1), m_state(SceneManagerState::Uninitialized),
		m_current_scene(nullptr), m_new_loaded_scene(nullptr), 
		m_loading_lock(gcnew Object()), m_swap_lock(gcnew Object()),
		m_swap_event(gcnew ManualResetEvent(false))
	{
	}
	void SceneManager::SetCurrent(Scene^ value)
	{
		if (value == nullptr)	// If clear scene
		{
			SetEmpty();
			return;
		}
		assert(!m_current_scene || value->ID() != m_current_scene->ID());
		// Set new scene
		Scene^ oldScene = m_current_scene;
		ThomasWrapper::Selection->UnselectGameObjects();
		m_current_scene = value;
		if (Application::currentProject)
			Application::currentProject->currentScenePath = m_current_scene->RelativeSavePath;

		// Trigger change
		OnCurrentSceneChanged(oldScene, value);

		// Cleanup
		if (oldScene)
			oldScene->~Scene();
	}
	bool SceneManager::LoadThreadWaiting()
	{
		Monitor::Enter(m_swap_lock);
		bool load_waiting = m_new_loaded_scene != nullptr;
		Monitor::Exit(m_swap_lock);
		return load_waiting;
	}


	void SceneManager::ListenToLoadProcess()
	{
		Monitor::Enter(m_swap_lock);
		bool is_loaded = m_new_loaded_scene != nullptr;
		Monitor::Exit(m_swap_lock);
		if (is_loaded)
		{
			SetCurrent(m_new_loaded_scene);
			m_new_loaded_scene = nullptr;
			m_swap_event->Set();
		}
	}

	void SceneManager::RestartCurrentScene()
	{
		// Assert: isLogicThread()
		Object^ lock = m_current_scene->GetGameObjectsLock();
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		
		Monitor::Enter(lock);
		m_current_scene->UnLoad();
		Monitor::Exit(lock);

		Scene^ scene = Scene::LoadScene(tempFile, m_ID_Counter++);
		SetCurrent(scene);

		try {
			System::IO::File::Delete(tempFile);
		}
		catch (Exception^ e)
		{
			Debug::LogError("Error in ThomasEngine::Scene. Msg: " + e->Message);
		}
	}

	void SceneManager::LogicThreadClearScene()
	{
		SetCurrent(gcnew Scene("void", m_ID_Counter++));
	}

	bool SceneManager::LoadScene(System::String ^ fullPath)
	{
		if (Monitor::TryEnter(m_loading_lock))
		{
			String^ t_name = Thread::CurrentThread->Name;
			if (t_name->Empty) t_name = "Unknown";
			Debug::Log("Thread: " + t_name + " began loading scene...");
			m_state = SceneManagerState::LoadingInProgress;
			Scene^ scene = Scene::LoadScene(fullPath, m_ID_Counter++);
			if (scene) // Only trigger if a scene is actually loaded
			{
				SyncSceneSwap(scene);
				Debug::Log("Scene: " + m_current_scene->Name + " finished loading!");
			}
			Monitor::Exit(m_loading_lock);
		}
		else {
			Debug::LogWarning("Error loading scene: Scene loading in progress...");
			return false;
		}
		return true;
	}

	bool SceneManager::NewScene(System::String ^ fullPath)
	{
		if (Monitor::TryEnter(m_loading_lock))
		{
			String^ t_name = Thread::CurrentThread->Name;
			if (t_name->Empty) t_name = "Unknown";
			Debug::Log("Thread: " + t_name + " created new scene.");
			m_state = SceneManagerState::LoadingInProgress;
			Scene^ scene = gcnew Scene(fullPath, m_ID_Counter++);
			SyncSceneSwap(scene); // Swap
			Monitor::Exit(m_loading_lock);
		}
		else {
			Debug::LogWarning("Error creating scene: Scene loading in progress...");
			return false;
		}
		return true;
	}

	void SceneManager::SyncSceneSwap(Scene^scene)
	{
		// Only trigger if a scene is actually loaded
		Monitor::Enter(m_swap_lock);
		m_swap_event->Reset();
		m_new_loaded_scene = scene;
		Monitor::Exit(m_swap_lock);
		m_swap_event->WaitOne();
		m_state = SceneManagerState::NormalState;
	}

	void SceneManager::SetEmpty()
	{
		if (Monitor::TryEnter(m_loading_lock))
		{
			m_state = SceneManagerState::LoadingInProgress;
			Scene^ scene = gcnew Scene("void", m_ID_Counter++);
			// Set swap state (lock preventing race conditions while setting swap parameters)
			SyncSceneSwap(scene);
			Monitor::Exit(m_loading_lock);
		}
		else {
			Debug::LogWarning("Error loading scene: Scene loading in progress...");
		}
	}
	bool SceneManager::IsAsyncLoading()
	{
		return m_state;
	}
	bool SceneManager::NoSceneExist()
	{
		return m_current_scene == nullptr; 
	}
	Scene^ SceneManager::CurrentScene::get()
	{
		return m_current_scene;
	}

}