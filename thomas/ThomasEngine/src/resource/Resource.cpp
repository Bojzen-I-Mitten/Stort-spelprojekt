#pragma unmanaged
#include <thomas\resource\Resource.h>
#pragma managed
#include "Resource.h"

#include "..\Application.h"
#include "../Project.h"
#include "../Scene.h"
#include "../ThomasManaged.h"

namespace ThomasEngine
{
	Resource::~Resource()
	{
	}
	String^ Resource::Path::get()
	{
		return m_path;
	}
	void Resource::Path::set(System::String ^ path)
	{
		m_path = path;
		m_nativePtr->setName(Utility::ConvertString(Name));
	}
	bool Resource::ContainsData()
	{
		return m_path->Length;
	}
	void Resource::asset_path::set(String^ value)
	{
		m_path = value->Replace("%THOMAS_DATA%", Application::editorAssets);
		m_path = m_path->Replace("%THOMAS_ASSETS%", Application::currentProject->assetPath);
	}

	String ^ Resource::GetAssetRelativePath()
	{
		if (m_path->Contains(Application::editorAssets)) return m_path->Replace(Application::editorAssets, "%THOMAS_DATA%");
		else return m_path->Replace(Application::currentProject->assetPath, "%THOMAS_ASSETS%");
		return "";
	}

	void Resource::Reload()
	{
		//System::Threading::Monitor::Enter(ThomasWrapper::CurrentScene->GetGameObjectsLock());
		m_nativePtr->Reload();
		//System::Threading::Monitor::Exit(ThomasWrapper::CurrentScene->GetGameObjectsLock());
	}

	void Resource::Rename(String^ newPath) {
		m_path = newPath;
		m_nativePtr->Rename(Utility::ConvertString(newPath));
		OnPropertyChanged("Name");
		OnRename();

	}

	bool Resource::operator ==(Resource^ a, Resource^ b)
	{
		if (Object::ReferenceEquals(nullptr, a))
			return Object::ReferenceEquals(nullptr, b);

		if (Object::ReferenceEquals(nullptr, b))
			return false;

		return a->m_nativePtr == b->m_nativePtr;
	}

	bool Resource::operator !=(Resource^ a, Resource^ b)
	{

		if (Object::ReferenceEquals(nullptr, a))
			return !Object::ReferenceEquals(nullptr, b);

		if (Object::ReferenceEquals(nullptr, b))
			return true;

		return a->m_nativePtr != b->m_nativePtr;


	}
}
