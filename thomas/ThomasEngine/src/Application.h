#pragma once

namespace ThomasEngine
{
	ref class Project;
	public ref class Application
	{
	public:
		delegate void CurrentProjectChangedEvent(Project^ newProject);
		static event CurrentProjectChangedEvent^ currentProjectChanged;
		static Project^ m_currentProject;
		static property System::String^ editorAssets {
			System::String^ get() { return System::IO::Path::GetFullPath(
				System::IO::Path::GetDirectoryName(System::Reflection::Assembly::GetExecutingAssembly()->Location) + "\\..\\Data"); }
		}

		static property Project^ currentProject
		{
			Project^ get() { return m_currentProject; }
			void set(Project^ value);
		}
	};
}
