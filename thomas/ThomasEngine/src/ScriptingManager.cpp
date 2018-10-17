#include "Utility.h"
#include "ScriptingManager.h"
#include "ThomasManaged.h"
#include "Debug.h"
#include "../src/system/SceneManager.h"

namespace ThomasEngine
{

	void ScriptingManger::LoadAssembly()
	{


		String^ tempFile = Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
		if (File::Exists(fsw->Path + "/Assembly.dll"))
		{
			scriptReloadStarted();
			String^ currentSavePath;
			if (ThomasWrapper::CurrentScene)
			{
				currentSavePath = ThomasWrapper::CurrentScene->RelativeSavePath;
				ThomasWrapper::CurrentScene->SaveSceneAs(tempFile);
			}

			//Delete the temp pdb file in the obj folder. Prevents the application from locking this file.
			if (File::Exists(objPath + "/Assembly.pdb"))
				File::Delete(objPath + "/Assembly.pdb");

			array<Byte>^ bytes = File::ReadAllBytes(fsw->Path + "/Assembly.dll");
			array<Byte>^ symbolBytes = File::ReadAllBytes(fsw->Path + "/Assembly.pdb");
			assembly = Assembly::Load(bytes, symbolBytes);
			if (ThomasWrapper::CurrentScene)
			{
				try
				{
					ThomasWrapper::Thomas->SceneManagerRef->LoadScene(tempFile);
					File::Delete(tempFile);
				}
				catch (Exception^ e) {
					String^ err = "Warning! ThomasEngine::ScriptingManager Failure to access temporary file when loading assembly. " + e->Message;
					Debug::Log(err);
				}
				ThomasWrapper::CurrentScene->RelativeSavePath = currentSavePath;
			}
		}
		//fsw->EnableRaisingEvents = true;
		scriptReloadFinished();
		shouldReload = false;

	}
	void ScriptingManger::ReloadIfNeeded() {
		if (shouldReload && !ThomasWrapper::IsLoading())
			LoadAssembly();
	}

	void ScriptingManger::OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e)
	{
		fsw->EnableRaisingEvents = false;
		//FileInfo^ fInfo = gcnew FileInfo(e->FullPath);
		//while (IsFileLocked(fInfo)) {
		//	Thread::Sleep(500);
		//}
		//Thread::Sleep(1000);
		shouldReload = true;
	}

}