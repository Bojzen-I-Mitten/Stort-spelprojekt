#include "ScriptingManager.h"
#include "ThomasManaged.h"

void ThomasEngine::ScriptingManger::LoadAssembly()
{


	String^ tempFile = Path::Combine(Environment::GetFolderPath(Environment::SpecialFolder::LocalApplicationData), "thomas/scene.tds");
	if (File::Exists(fsw->Path + "/Assembly.dll"))
	{
		
		Scene::savingEnabled = false;
		scriptReloadStarted();
		String^ currentSavePath;
		if (Scene::CurrentScene)
		{
			currentSavePath = Scene::CurrentScene->RelativeSavePath;
			Scene::SaveSceneAs(Scene::CurrentScene, tempFile);
		}

		//Delete the temp pdb file in the obj folder. Prevents the application from locking this file.
		if (File::Exists(objPath + "/Assembly.pdb"))
			File::Delete(objPath + "/Assembly.pdb");

		array<Byte>^ bytes = File::ReadAllBytes(fsw->Path + "/Assembly.dll");
		array<Byte>^ symbolBytes = File::ReadAllBytes(fsw->Path + "/Assembly.pdb");
		assembly = Assembly::Load(bytes, symbolBytes);
		if (Scene::CurrentScene)
		{

			Scene^ oldScene = Scene::CurrentScene;
			Scene::CurrentScene = Scene::LoadScene(tempFile);
			oldScene->UnLoad();

			File::Delete(tempFile);
			Scene::CurrentScene->RelativeSavePath = currentSavePath;			
		}
	}
	//fsw->EnableRaisingEvents = true;
	Scene::savingEnabled = true;
	scriptReloadFinished();
	shouldReload = false;
	
}

void ThomasEngine::ScriptingManger::OnChanged(System::Object ^sender, System::IO::FileSystemEventArgs ^e)
{
	fsw->EnableRaisingEvents = false;
	//FileInfo^ fInfo = gcnew FileInfo(e->FullPath);
	//while (IsFileLocked(fInfo)) {
	//	Thread::Sleep(500);
	//}
	//Thread::Sleep(1000);
	shouldReload = true;
}
