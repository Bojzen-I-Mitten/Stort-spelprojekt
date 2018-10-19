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
		Loading
	};
	
	ref class SceneManager;
	ref class Scene;

	ref class ThomasSelection;
	public ref class ThomasWrapper
	{
	private:
		static ThomasWrapper^ s_SYS = gcnew ThomasWrapper();
		static bool inEditor = false;
		static float cpuTime = 0.0f;
		static bool showStatistics = false;
		static bool renderingEditor = true;
		static Thread^ mainThread;
		static Thread^ renderThread;
		static RunningState playing = RunningState::Loading;
		static ManualResetEvent^ RenderFinished;
		static ManualResetEvent^ UpdateFinished;
		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection;
	private:	// Thomas System variables.
		SceneManager^ m_scene;
		static void Stop();
	public:

		property SceneManager^ SceneManagerRef
		{
			SceneManager^ get();
		}

	public:	// Static sys
		delegate void StartPlayEvent();
		delegate void StopPlayingEvent();
		delegate void PausePlayEvent();
		static event StartPlayEvent^ OnStartPlaying;
		static event StopPlayingEvent^ OnStopPlaying;

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

		static void Start();


		static void StartRenderer();

		static void CopyCommandList();

		static void StartEngine();

		static void Exit();

		static void CreateThomasWindow(IntPtr hWnd, bool isEditor);

		static void eventHandler(IntPtr hWnd, int msg, IntPtr wParam, IntPtr lParam);

		static void Resize(IntPtr hWnd, double width, double height);

		static void Update();

		static Guid selectedGUID;
		static void Play();

		static bool IsPlaying();
		static bool IsLoading();
		
		static void SetEditorGizmoManipulatorOperation(ManipulatorOperation op);

		static ManipulatorOperation GetEditorGizmoManipulatorOperation();

		static void ToggleEditorGizmoManipulatorMode();

	
		static bool InEditor();

		static void ToggleEditorRendering();
		static void TogglePhysicsDebug();
	public:

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
