#include "Project.h"

String^ ThomasEngine::Project::currentScenePath::get()
{
	if (m_relativeCurrentScenePath)
		return m_relativeCurrentScenePath;
	else 
		return nullptr;
}

void ThomasEngine::Project::currentScenePath::set(String^ value)
{
	if (value)
		m_relativeCurrentScenePath = value;
	Save();
}
