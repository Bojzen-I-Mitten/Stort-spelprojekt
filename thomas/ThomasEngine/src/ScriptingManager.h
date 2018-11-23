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

		/* Public access function for assembly reload. Assure synchronous state!!!
		*/
		static void ForceReloadAssembly();

	internal:
		static void Init();

		static void OnCurrentProjectChanged(Project^ newProject);
		static Assembly^ GetAssembly();

		delegate void OnChanged(Object^ sender, FileSystemEventArgs e);
		

		static bool IsFileLocked(FileInfo^ file);

		/* Reload script assembly if necessary. Reloading Updates the script .dll
		forceReload	<<	Don't care if necessary or not
		*/
		static void ReloadAssembly(bool forceReload);

		static void OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e);

		/* Load new assembly (call ReloadAssembly if not initiating new assembly)
		*/
		static void LoadAssembly();
	};
}