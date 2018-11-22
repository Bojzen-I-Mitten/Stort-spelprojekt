// ThomasManaged.h

#pragma once

#pragma managed

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Threading;

namespace ThomasEngine {

#define _THOMAS_SCENE_LOCK

	enum RunningState
	{
		Editor,
		Running,
		UnInitialized
	};
	enum ThomasStateCommand
	{
		NoCommand = 0,
		PlayIssued,
		StopIssued,
		ReplayIssued
	};
	enum ThomasSystemMode
	{
		EditorMode,
		Standalone
	};
	
	ref class SceneManager;
	ref class Scene;

	ref class ThomasSelection;
	ref class GameObjectManager;
	ref class CommandQueue;
	interface class ICommand;
	public ref class ThomasWrapper
	{
	private:

		static GameObjectManager^ s_GameObjectManager;

		static ThomasWrapper^ s_SYS;
		static bool inEditor = false;
		static float logicTime = 0.0f;
		static float renderTime = 0.0f;
		static bool showStatistics = false;
		static RunningState playing = RunningState::UnInitialized;
		
		/* Threading
		*/
		static Thread^ logicThread;
		static Thread^ renderThread;
		static System::Windows::Threading::Dispatcher^ mainThreadDispatcher;
		delegate void MainThreadDelegate();
		

		static ManualResetEvent^ RenderFinished, ^UpdateFinished;
		static ManualResetEvent^ EditorWindowLoaded, ^GameWindowLoaded;
		static ManualResetEvent^ StateCommandProcessed;
		static ManualResetEvent^ WaitLogOutput;
		static ThomasStateCommand IssuedStateCommand = ThomasStateCommand::NoCommand;


		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection;

		static void Play();
		static void StopPlay();
		static void ProcessStateCommand();
		static void SynchronousExecution();

		static void DumpProfilerLog(System::Object^ stateInfo);
		static void SampleRam(System::Object ^ stateInfo);


		ThomasWrapper();
		~ThomasWrapper();

	private:	// Thomas System variables.
		SceneManager^ m_scene;
		CommandQueue^ m_engineCommands;
		Object^ m_sceneLock;

		static void LoadEditorAssets();
	public:

		property SceneManager^ SceneManagerRef
		{
			SceneManager^ get();
		}

#ifdef _EDITOR
		delegate void StartPlayEvent();
		delegate void StopPlayingEvent();
		static event StartPlayEvent^ OnStartPlaying;
		static event StopPlayingEvent^ OnStopPlaying;
#endif

	public:	// Static sys

		enum class ManipulatorOperation {
			TRANSLATE,
			ROTATE,
			SCALE
		};
		static void Start(bool editor);
		static void Start();
		static void MainThreadUpdate();
		static void StartRenderer();
#ifdef _EDITOR
		/* Triggered event on game window load, only called when editor is used
		*/
		static void ThomasGameWindowLoaded();
		/* Triggered event on editor window load, only called when editor is used
		*/
		static void ThomasEditorWindowLoaded();
#endif

		static property ThomasWrapper^ Thomas
		{
			ThomasWrapper^ get();
		}
		static property Scene^ CurrentScene
		{
			Scene^ get();
		}
		/* Enter a lock synchonizing state with thomas logic loop, use with care (inefficient).
		 * Lock can be disabled by undef _THOMAS_SCENE_LOCK
		*/
		static void ENTER_SYNC_STATELOCK();
		/* Exit the synchronous state lock
		*/
		static void EXIT_SYNC_STATELOCK();
		static void IssueCommand(ICommand^ cmd);
		/* Issue a specific state switch command to thomas
		*/
		static void IssueStateCommand(ThomasStateCommand cmd);
		/* Issue a play state command to thomas
		*/
		static void IssuePlay();
		/* Issue a stop playing state command to thomas
		*/
		static void IssueStopPlay();


		static void CopyCommandList();

		static void StartEngine();

		static void Exit();

		static void CreateThomasWindow(IntPtr hWnd, bool isEditor);

		static void eventHandler(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam);

		static void Resize(IntPtr hWnd, double width, double height);

		static void Update();


		static Guid selectedGUID;

		static bool IsPlaying();
		
		static void SetEditorGizmoManipulatorOperation(ManipulatorOperation op);

		static ManipulatorOperation GetEditorGizmoManipulatorOperation();

		static void ToggleEditorGizmoManipulatorMode();

	
		static bool InEditor();
		
	public:
		static bool RenderEditor = true;
		static property bool RenderPhysicsDebug
		{
			bool get();
			void set(bool value);
		}
		static property float FrameRate
		{
			float get();
		}
		static ThomasSelection^ getSelection() { return s_Selection; }
		static property ThomasSelection^ Selection {
			ThomasSelection^ get() { return s_Selection; }
		}

	};
}
