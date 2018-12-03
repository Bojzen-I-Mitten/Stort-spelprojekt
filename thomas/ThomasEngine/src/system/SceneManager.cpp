#pragma unmanaged

#include <assert.h>

#pragma managed
using namespace System::IO;
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
			value = CreateEmpty();
		assert(!m_current_scene || value->ID() != m_current_scene->ID());
		// Set new scene
		Scene^ oldScene = m_current_scene;
		ThomasWrapper::Selection->UnselectGameObjects();
		m_current_scene = value;
		if (!m_temporarySwap && Application::currentProject)
			Application::currentProject->currentScenePath = m_current_scene->RelativeSavePath;

		// Run initiation sequence
		m_current_scene->InitiateScene();

		// Trigger change
#ifdef _EDITOR
		OnCurrentSceneChanged(oldScene, value);
#endif
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
		String^ tempFile = System::IO::Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), Local_Temp_Copy_Path);
		

		Scene^ scene = Scene::LoadScene(tempFile, m_ID_Counter++);
		m_temporarySwap = true;
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
		return LoadScene(fullPath, false);
	}

	bool SceneManager::LoadScene(System::String ^ fullPath, bool isTemporary)
	{
		bool success = false;
		if (Monitor::TryEnter(m_loading_lock))
		{
			String^ t_name = Thread::CurrentThread->Name;
			if (t_name->Empty) t_name = "Unknown";
			Debug::Log("Thread: " + t_name + " began loading scene...");
			m_state = SceneManagerState::LoadingInProgress;
			Scene^ scene = Scene::LoadScene(fullPath, m_ID_Counter++);
			if (scene) // Only trigger if a scene is actually loaded
			{
				SyncSceneSwap(scene, isTemporary);
				Debug::Log("Scene: " + m_current_scene->Name + " finished loading!");
				success = true;
			}
			m_state = SceneManagerState::NormalState;

			Monitor::Exit(m_loading_lock);
		}
		else {
			Debug::LogWarning("Error loading scene: Scene loading in progress...");
			return success;
		}
		return success;
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
			SyncSceneSwap(scene, false); // Swap
			m_state = SceneManagerState::NormalState;
			Monitor::Exit(m_loading_lock);
		}
		else {
			Debug::LogWarning("Error creating scene: Scene loading in progress...");
			return false;
		}
		return true;
	}

	SceneManager::TempCopy^ SceneManager::StoreTempCopy()
	{
		if (ThomasWrapper::CurrentScene)
		{
			TempCopy^ cpy = gcnew TempCopy(Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), Local_Temp_Copy_Path));
			ThomasWrapper::CurrentScene->SaveSceneAs(cpy->Temp_Path);
			return cpy;
		}
		return nullptr;
	}

	void SceneManager::LoadTempCopy(TempCopy ^ copy)
	{
		if (copy)
		{
			ThomasWrapper::Thomas->SceneManagerRef->LoadScene(copy->Temp_Path, true);
			File::Delete(copy->Temp_Path);
		}
		else
			Debug::LogWarning("Temporary copy was null, failed re-loading scene.");
	}

	void SceneManager::SyncSceneSwap(Scene^scene, bool temporary)
	{
		// Set swap state (lock preventing race conditions while setting swap parameters)
		Monitor::Enter(m_swap_lock);
		m_swap_event->Reset();
		m_new_loaded_scene = scene;
		m_temporarySwap = temporary;
		Monitor::Exit(m_swap_lock);
		m_swap_event->WaitOne();
	}
	Scene^ SceneManager::CreateEmpty()
	{
		return gcnew Scene("void", m_ID_Counter++);
	}

	void SceneManager::SetEmpty()
	{
		if (Monitor::TryEnter(m_loading_lock))
		{
			m_state = SceneManagerState::LoadingInProgress;
			Scene^ scene = CreateEmpty();
			SyncSceneSwap(scene, false);
			m_state = SceneManagerState::NormalState;
			Monitor::Exit(m_loading_lock);
		}
		else {
			Debug::LogWarning("Error loading scene: Scene loading in progress...");
		}
	}
	bool SceneManager::IsAsyncLoading()
	{
		return m_state == LoadingInProgress;
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