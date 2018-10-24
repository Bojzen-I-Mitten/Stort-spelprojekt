#include "Utility.h"
#include "ScriptingManager.h"
#include "ThomasManaged.h"
#include "Debug.h"
#include "../src/system/SceneManager.h"

namespace ThomasEngine
{

	void ScriptingManger::LoadAssembly()
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
	void ScriptingManger::ReloadAssembly(bool forceReload) {
		if ((forceReload || shouldReload) &&								// Check if force reload
			!ThomasWrapper::Thomas->SceneManagerRef->IsAsyncLoading())		// Ensure thomas is not in loading process
		{
			// Store current scene
			String^ tempFile = Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "ThomasEditor/scene.tds");
			String^ currentSavePath;
			if (ThomasWrapper::CurrentScene)
			{
				currentSavePath = ThomasWrapper::CurrentScene->RelativeSavePath;
				ThomasWrapper::CurrentScene->SaveSceneAs(tempFile);
			}

			// Reload assembly
			LoadAssembly();

			// Reload scene
			if (ThomasWrapper::CurrentScene)
			{
				try
				{
					ThomasWrapper::Thomas->SceneManagerRef->LoadScene(tempFile, true);
					File::Delete(tempFile);
				}
				catch (Exception^ e) {
					String^ err = "Warning! ThomasEngine::ScriptingManager Failure to access temporary file when loading assembly. " + e->Message;
					Debug::Log(err);
				}
				ThomasWrapper::CurrentScene->RelativeSavePath = currentSavePath;
			}
		}
	}

	void ScriptingManger::OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e)
	{
		fsw->EnableRaisingEvents = false;
		shouldReload = true;
	}

}