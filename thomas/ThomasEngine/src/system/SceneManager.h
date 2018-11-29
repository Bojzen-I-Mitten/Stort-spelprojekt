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
	public:
		ref class TempCopy
		{
		public:
			System::String^ Temp_Path;
			TempCopy(System::String^ Path)	: 
				Temp_Path(Path)
			{}
		};
	private:
		SceneManagerState m_state;
		uint32_t m_ID_Counter;

		Scene^ m_current_scene;
		Scene^ m_new_loaded_scene;
		bool m_temporarySwap;
		Object^ m_loading_lock;
		Object^ m_swap_lock;
		ManualResetEvent^ m_swap_event;

		void SetCurrent(Scene^ scene);
		/* Wait process for async. loading thread. */
		void SyncSceneSwap(Scene ^ scene, bool temporary);

		Scene ^ CreateEmpty();

		static System::String^ Local_Temp_Copy_Path = gcnew System::String("ThomasEditor/scene.tds");

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
		bool LoadScene(System::String^ fullPath, bool isTemporary);
		bool NewScene(System::String^ fullPath);
		TempCopy ^ StoreTempCopy();
		/* Load temporary file (Can throw exception)
		*/ 
		void LoadTempCopy(TempCopy^ copy);



		delegate void CurrentSceneChanged(Scene^ oldScene, Scene^ newScene);
		event CurrentSceneChanged^ OnCurrentSceneChanged;

		property Scene^ CurrentScene {
			Scene^ get();
		}
	};

}