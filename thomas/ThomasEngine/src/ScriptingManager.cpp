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
		if ((forceReload || shouldReload) &&								// Check if force reload
			!ThomasWrapper::Thomas->SceneManagerRef->IsAsyncLoading())		// Ensure thomas is not in loading process
		{
			// Store current scene

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

	void ScriptingManager::OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e)
	{
		fsw->EnableRaisingEvents = false;
		shouldReload = true;
	}

}