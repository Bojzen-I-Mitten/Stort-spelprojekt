// ThomasManaged.h

#pragma once


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Threading;

namespace ThomasEngine {


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
		RestartIssued
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
	public ref class ThomasWrapper
	{
	private:

		static GameObjectManager^ s_GameObjectManager;

		static ThomasWrapper^ s_SYS = gcnew ThomasWrapper();
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

		static ManualResetEvent^ RenderFinished;
		static ManualResetEvent^ UpdateFinished;
		static ManualResetEvent^ StateCommandProcessed;
		static ManualResetEvent^ WaitLogOutput;
		static ThomasStateCommand IssuedStateCommand = ThomasStateCommand::NoCommand;


		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection;

		static void Play();
		static void StopPlay();
		static void ProcessCommand();
		static void SynchronousExecution();

		static void DumpProfilerLog(System::Object^ stateInfo);
		static void SampleRam(System::Object ^ stateInfo);


	private:	// Thomas System variables.
		SceneManager^ m_scene;
	public:

		property SceneManager^ SceneManagerRef
		{
			SceneManager^ get();
		}
		static void Start(bool editor);
		static void Start();
		static void MainThreadUpdate();
		static void StartRenderer();


	public:	// Static sys

		enum class ManipulatorOperation {
			TRANSLATE,
			ROTATE,
			SCALE
		};


		static property ThomasWrapper^ Thomas
		{
			ThomasWrapper^ get();
		}
		static property Scene^ CurrentScene
		{
			Scene^ get();
		}

		static void IssueStateCommand(ThomasStateCommand cmd);
		static void IssuePlay();
		static void IssueStopPlay();
		static void IssueRestart();


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
