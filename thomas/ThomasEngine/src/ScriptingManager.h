#pragma once

#include "Scene.h"
#include "object\GameObject.h"
#include "Application.h"
#include "Project.h"
using namespace System;
using namespace System::IO;
using namespace System::Reflection;
namespace ThomasEngine
{
	public ref class ScriptingManager
	{
	private:
		static FileSystemWatcher^ fsw;
		static String^ objPath;
		static Assembly^ assembly;
		static bool shouldReload = false;
	public:
		
		delegate void ScriptReloadBegin();
		static event ScriptReloadBegin^ scriptReloadStarted;
		delegate void ScriptReloadEnded();
		static event ScriptReloadEnded^ scriptReloadFinished;
	
		static void Init() {
			
			fsw = gcnew FileSystemWatcher();
			fsw->Filter = "Assembly.dll";
			fsw->Changed += gcnew FileSystemEventHandler(&ScriptingManager::OnChanged);
			Application::currentProjectChanged += gcnew Application::CurrentProjectChangedEvent(&ScriptingManager::OnCurrentProjectChanged);
		}

		static void OnCurrentProjectChanged(Project^ newProject) {
			String^ assemblyFolderPath = newProject->assembly;
			objPath = newProject->path + "/obj";
#ifdef _DEBUG
			assemblyFolderPath += "/Debug";
			objPath += "/Debug";
#else
			assemblyFolderPath += "/Release";
			objPath += "/Release";
#endif
			fsw->EnableRaisingEvents = false;
			fsw->Path = assemblyFolderPath;
			//fsw->EnableRaisingEvents = true;
			LoadAssembly();
		}
		static Assembly^ GetAssembly()
		{
			return assembly;
		}

		delegate void OnChanged(Object^ sender, FileSystemEventArgs e);
		

		static bool IsFileLocked(FileInfo^ file)
		{
			FileStream^ stream;
			try {
				stream = file->Open(FileMode::Open, FileAccess::ReadWrite, FileShare::None);
			}
			catch (IOException^) {
				return true;
			}
			finally{
				if (stream)
					stream->Close();
			}
			return false;
		}

		/* Reload assembly
		forceReload	<<	Don't care if necessary or not
		*/
		static void ReloadAssembly(bool forceReload);

		static void OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e);

	internal:

		/* Load new assembly (call ReloadAssembly if not initiating new assembly)
		*/
		static void LoadAssembly();
	};
}