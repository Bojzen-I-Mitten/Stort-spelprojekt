#include "Application.h"
#include "Scene.h"
void ThomasEngine::Application::currentProject::set(ThomasEngine::Project^ value)
{
	m_currentProject = nullptr;
	if (Scene::CurrentScene) {
		Scene::CurrentScene->UnLoad();
	}
	Scene::CurrentScene = nullptr;
	currentProjectChanged(value);
	m_currentProject = value;
	Scene^ currentScene;
	if (m_currentProject->currentScenePath)
		currentScene = Scene::LoadScene(m_currentProject->assetPath + "\\" + m_currentProject->currentScenePath);

	if (currentScene)
		Scene::CurrentScene = currentScene;
	else
		Scene::CurrentScene = gcnew Scene("scene");
}