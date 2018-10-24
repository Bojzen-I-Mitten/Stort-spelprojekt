#pragma once
#pragma unmanaged


#pragma managed

#include "../Scene.h"
using namespace System::Threading;

namespace ThomasEngine 
{
	ref class Scene;

	enum SceneManagerState
	{
		NormalState = 0,
		LoadingInProgress = 1,	// Marked for asynchronous loading, scene loading is locked
		Uninitialized =  2
	};

	public ref class SceneManager
	{
	private:
		SceneManagerState m_state;
		uint32_t m_ID_Counter;

		Scene^ m_current_scene;
		Scene^ m_new_loaded_scene;
		Object^ m_loading_lock;
		Object^ m_swap_lock;
		ManualResetEvent^ m_swap_event;

		void SetCurrent(Scene^ scene);
		/* Wait process for async. loading thread. */
		void SyncSceneSwap(Scene ^ scene);

		Scene ^ CreateEmpty();


	internal:
		/* Called by logic process to swap scene.
		*/
		void ListenToLoadProcess();
		/*	Check if loading process is ready for scene swap.
		*/ 
		bool LoadThreadWaiting();
		/*	Reload system temp file. Does not enter swap state, and should only be called from logic thread.
		*/
		void RestartCurrentScene();
		/* Clear scene for the logic thread.
		*/
		void LogicThreadClearScene();

		/* Check if a process initiated a loading process.
		*/
		bool IsAsyncLoading();
		/* Check if a no scene are activated. 
		*/
		bool NoSceneExist();

	
	public:
		/*	Clear out current scene by setting empty. 
		*/
		void SetEmpty();


		SceneManager();
		/* Load scene asynchronous
		*/
		bool LoadScene(System::String^ fullPath);
		bool NewScene(System::String^ fullPath);



		delegate void CurrentSceneChanged(Scene^ oldScene, Scene^ newScene);
		event CurrentSceneChanged^ OnCurrentSceneChanged;

		property Scene^ CurrentScene {
			Scene^ get();
		}
	};

}