// ThomasManaged.h

#pragma once


#pragma unmanaged

#include <thomas\ThomasCore.h>
#include <thomas\Window.h>
#include <thomas\ThomasTime.h>
#include <thomas\Input.h>
#include <thomas\resource\Shader.h>
#include <thomas\graphics\Renderer.h>
#include <thomas\editor\gizmos\Gizmos.h>
#pragma managed
//#include <Sound.h>
#include "resource\Resources.h"
#include "object\GameObject.h"
#include "object\component\Transform.h"
#include "Scene.h"
#include "ScriptingManager.h"
#include "ThomasSelection.h"

using namespace System;
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace thomas;


namespace ThomasEngine {


	public ref class ThomasWrapper
	{
	private:

		static bool updateEditor = false;

		static Thread^ mainThread;
		static Thread^ renderThread;
		static bool playing = false;	
		static ManualResetEvent^ RenderFinished;
		static ManualResetEvent^ UpdateFinished;
		static ObservableCollection<String^>^ s_OutputLog = gcnew ObservableCollection<String^>();
		static ThomasSelection^ s_Selection = gcnew ThomasSelection();
	public:
		delegate void UpdateEditor();
		static event UpdateEditor^ OnEditorUpdate;

		enum class ManipulatorOperation {
			TRANSLATE,
			ROTATE,
			SCALE
		};

		static void Start();

		static void UpdateEditor();

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

	public:

		static property float FrameRate
		{
			float get() { return float(thomas::ThomasTime::GetFPS()); }
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

//namespace msclr 
//{
//	namespace interop 
//	{
//		template<>
//		inline ThomasEngine::GameObject^ marshal_as<ThomasEngine::GameObject^, thomas::object::GameObject>(const thomas::object::GameObject& from)
//		{
//			marshal_context context;
//			String^ temp = gcnew String("");
//			temp = context.marshal_as<String^>(from.GetName().c_str());
//			//std::string temp = from.GetName();
//			//String^ name = gcnew(from;
//
//			marshal_context context;
//			return ThomasEngine::GameObject::Find(context.marshal_as<String^, std::string>(from.GetName()));
//		}
//	}
//}
