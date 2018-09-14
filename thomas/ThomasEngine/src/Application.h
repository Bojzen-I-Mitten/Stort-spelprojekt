#pragma once

#include "Project.h"

namespace ThomasEngine
{
	public ref class Application
	{
	public:
		delegate void CurrentProjectChangedEvent(Project^ newProject);
		static event CurrentProjectChangedEvent^ currentProjectChanged;
		static Project^ m_currentProject;
		static property String^ editorAssets {
			String^ get() { return IO::Path::GetFullPath(
				IO::Path::GetDirectoryName(Reflection::Assembly::GetExecutingAssembly()->Location) + "\\..\\Data"); }
		}

		static property Project^ currentProject
		{
			Project^ get() { return m_currentProject; }
			void set(Project^ value);
		}
	};
}
