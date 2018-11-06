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
		StopIssued
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
		static float cpuTime = 0.0f;
		static bool showStatistics = false;
		static RunningState playing = RunningState::UnInitialized;
		
		/* Threading
		*/
		static Thread^ logicThread;
		static Thread^ renderThread;
		static System::Windows::Threading::Dispatcher^ mainThreadDispatcher;
		delegate void MainThreadDelegate();
		

		static ManualResetEvent^ RenderFinished;
		static ManualResetEvent^ UpdateFinished;
		static ManualResetEvent^ StateCommandProcessed;
		static ThomasStateCommand IssuedStateCommand = ThomasStateCommand::NoCommand;


		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection;

		static void Play();
		static void StopPlay();
		static void ProcessCommand();
		static void SynchronousExecution();

		ThomasWrapper();
		~ThomasWrapper();

	private:	// Thomas System variables.
		SceneManager^ m_scene;
		CommandQueue^ m_engineCommands;
		Object^ m_sceneLock;
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


		static property ThomasWrapper^ Thomas
		{
			ThomasWrapper^ get();
		}
		static property Scene^ CurrentScene
		{
			Scene^ get();
		}
		static void ENTER_SYNC_STATELOCK();
		static void EXIT_SYNC_STATELOCK();
		static void IssueCommand(ICommand^ cmd);
		static void IssueStateCommand(ThomasStateCommand cmd);
		static void IssuePlay();
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
