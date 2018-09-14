#include "Resource.h"
#include "..\Application.h"
#include "../Scene.h"
void ThomasEngine::Resource::asset_path::set(String^ value)
{
	m_path = value->Replace("%THOMAS_DATA%", Application::editorAssets);
	m_path = m_path->Replace("%THOMAS_ASSETS%", Application::currentProject->assetPath);
}

String ^ ThomasEngine::Resource::GetAssetRelativePath()
{
	if (m_path->Contains(Application::editorAssets)) return m_path->Replace(Application::editorAssets, "%THOMAS_DATA%");
	else return m_path->Replace(Application::currentProject->assetPath, "%THOMAS_ASSETS%");
}

void ThomasEngine::Resource::Reload()
{
	System::Threading::Monitor::Enter(Scene::CurrentScene->GetGameObjectsLock());
	m_nativePtr->Reload();
	System::Threading::Monitor::Exit(Scene::CurrentScene->GetGameObjectsLock());
}