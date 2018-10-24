// ThomasManaged.h

#pragma once


using namespace System;
using namespace System::Collections::Generic;
using namespace System::Collections::ObjectModel;
using namespace System::Threading;

namespace ThomasEngine {
	ref class ThomasSelection;
	public ref class ThomasWrapper
	{
	private:
		static bool shouldStop = false;
		static bool inEditor = false;
		static float cpuTime = 0.0f;
		static bool showStatistics = false;
		static bool renderingEditor = true;
		static Thread^ mainThread;
		static Thread^ renderThread;
		static bool playing = false;	
		
		static ManualResetEvent^ UpdateFinished;
		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection;
	private:
		static void Stop();
	public:
		static ManualResetEvent^ RenderFinished;

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

		static void Start();
		static void Start(bool editor);


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

		static void IssueStop();

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
