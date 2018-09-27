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
		static bool renderingEditor = true;
		static Thread^ mainThread;
		static Thread^ renderThread;
		static bool playing = false;	
		static ManualResetEvent^ RenderFinished;
		static ManualResetEvent^ UpdateFinished;
		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection;
	public:
		enum class ManipulatorOperation {
			TRANSLATE,
			ROTATE,
			SCALE
		};

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

		static void Stop();

		static void SetEditorGizmoManipulatorOperation(ManipulatorOperation op);

		static ManipulatorOperation GetEditorGizmoManipulatorOperation();

		static void ToggleEditorGizmoManipulatorMode();

		static void UpdateLog();

		static void ToggleEditorRendering();
		static void TogglePhysicsDebug();
	public:

		static property float FrameRate
		{
			float get();
		}
		static property ObservableCollection<String^>^ OutputLog
		{
			ObservableCollection<String^>^ get() { return s_OutputLog; }
		}
		static ThomasSelection^ getSelection() { return s_Selection; }
		static property ThomasSelection^ Selection {
			ThomasSelection^ get() { return s_Selection; }
		}

	};
}
