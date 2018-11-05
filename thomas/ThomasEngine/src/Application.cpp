#include "Application.h"
#include "Scene.h"
#include "Project.h"
#include "system/SceneManager.h"
#include "ThomasManaged.h"
#include "system/SceneManager.h"
#include "resource/Resources.h"

void ThomasEngine::Application::currentProject::set(ThomasEngine::Project^ value)
{
	m_currentProject = value;
	Resources::LoadAll(m_currentProject->assetPath);
	currentProjectChanged(value);

	// Load scene
	if (m_currentProject->currentScenePath)
		ThomasWrapper::Thomas->SceneManagerRef->LoadScene(m_currentProject->assetPath + "\\" + m_currentProject->currentScenePath);
	else
		ThomasWrapper::Thomas->SceneManagerRef->SetEmpty();


}