#include "Utility.h"
#include "ScriptingManager.h"
#include "ThomasManaged.h"
#include "Debug.h"
#include "../src/system/SceneManager.h"

namespace ThomasEngine
{

	void ScriptingManager::LoadAssembly()
	{
		if (File::Exists(fsw->Path + "/Assembly.dll"))
		{
			scriptReloadStarted();

			//Delete the temp pdb file in the obj folder. Prevents the application from locking this file.
			if (File::Exists(objPath + "/Assembly.pdb"))
				File::Delete(objPath + "/Assembly.pdb");

			array<Byte>^ bytes = File::ReadAllBytes(fsw->Path + "/Assembly.dll");
			array<Byte>^ symbolBytes = File::ReadAllBytes(fsw->Path + "/Assembly.pdb");
			assembly = Assembly::Load(bytes, symbolBytes);
		}
		//fsw->EnableRaisingEvents = true;
		scriptReloadFinished();
		shouldReload = false;

	}
	void ScriptingManager::ReloadAssembly(bool forceReload) {
		// Ensure thomas is in a proper state: Not running or load in process
		if (ThomasWrapper::IsPlaying() || ThomasWrapper::Thomas->SceneManagerRef->IsAsyncLoading())
			return;
		// If needed
		if (forceReload || shouldReload)
		{
			// Store current scene
			Debug::Log("Reloading assembly...");
			SceneManager::TempCopy^ tmp = ThomasWrapper::Thomas->SceneManagerRef->StoreTempCopy();

			// Reload assembly
			LoadAssembly();

			// Reload scene
			try {
				ThomasWrapper::Thomas->SceneManagerRef->LoadTempCopy(tmp);
			}
			catch (Exception^ e) {
				String^ err = "Warning! ThomasEngine::ScriptingManager Failure to access temporary file when loading assembly. " + e->Message;
				Debug::Log(err);
			}
		}
	}
	void ScriptingManager::ForceReloadAssembly()
	{
		ReloadAssembly(true);
	}

	
	void ScriptingManager::Init() {

		fsw = gcnew FileSystemWatcher();
		fsw->Filter = "Assembly.dll";
		fsw->Changed += gcnew FileSystemEventHandler(&ScriptingManager::OnChanged);
		Application::currentProjectChanged += gcnew Application::CurrentProjectChangedEvent(&ScriptingManager::OnCurrentProjectChanged);
	}

	void ScriptingManager::OnCurrentProjectChanged(Project^ newProject) 
	{
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
	Assembly^ ScriptingManager::GetAssembly()
	{
		return assembly;
	}

	delegate void OnChanged(Object^ sender, FileSystemEventArgs e);


	bool ScriptingManager::IsFileLocked(FileInfo^ file)
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


	void ScriptingManager::OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e)
	{
		fsw->EnableRaisingEvents = false;
		shouldReload = true;
	}

}