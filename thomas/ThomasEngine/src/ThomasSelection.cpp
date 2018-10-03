#pragma unmanaged
#include <thomas\editor\EditorCamera.h>
#pragma managed
#include "ThomasSelection.h"
#include "object\GameObject.h"

using namespace System::Threading;
namespace ThomasEngine {

	ThomasSelection::ThomasSelection()
		: m_SelectedGameObjects(gcnew ObservableCollection<GameObject^>()), m_lock(gcnew Object())
	{
	}

	ThomasSelection::~ThomasSelection()
	{
	}


	void ThomasSelection::SelectGameObject(GameObject^ gObj)
	{
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		try {
			m_SelectedGameObjects->Clear();
			m_SelectedGameObjects->Add(gObj);
			thomas::editor::EditorCamera::Instance()->SelectObject((thomas::object::GameObject*)gObj->nativePtr);
		}
		finally	{
			Monitor::Exit(m_lock);
		}
	}

	void ThomasSelection::SelectGameObject(System::Guid guid)
	{
		if (guid != Guid::Empty)
		{
			GameObject^ gObj = (GameObject^)ThomasEngine::Object::Find(guid);
			if (gObj)
				SelectGameObject(gObj);
		}
	}

	void ThomasSelection::UnSelectGameObject(GameObject ^ gObj)
	{
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		try {
		m_SelectedGameObjects->Remove(gObj);
		thomas::editor::EditorCamera::Instance()->UnselectObject((thomas::object::GameObject*)gObj->nativePtr);
		}	
		finally	{
			Monitor::Exit(m_lock);
		}
	}

	void ThomasSelection::UnselectGameObjects()
	{
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		try {
		m_SelectedGameObjects->Clear();
		thomas::editor::EditorCamera::Instance()->SelectObject(nullptr);
		}	
		finally	{
			Monitor::Exit(m_lock);
		}
	}

	void ThomasSelection::render() 
	{
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		try {
			for each(ThomasEngine::GameObject^ gameObject in m_SelectedGameObjects)
			{
				if (gameObject->GetActive())
					gameObject->RenderSelectedGizmos();
			}
		}	
		finally	{
			Monitor::Exit(m_lock);
		}
	}


	void ThomasSelection::UpdateSelectedObjects() {
		List<GameObject^> tempSelectedGameObjects;
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		try {
			for (thomas::object::GameObject* gameObject : thomas::editor::EditorCamera::Instance()->GetSelectedObjects())
			{
				GameObject^ gObj = (GameObject^)ThomasEngine::Object::GetObject(gameObject);
				if (gObj)
					tempSelectedGameObjects.Add(gObj);
			}
			if (tempSelectedGameObjects.Count == m_SelectedGameObjects->Count)
			{
				if (tempSelectedGameObjects.Count > 0)
					if (tempSelectedGameObjects[0] != m_SelectedGameObjects[0])
					{
						m_SelectedGameObjects->Clear();
						for each(GameObject^ gObj in tempSelectedGameObjects)
							m_SelectedGameObjects->Add(gObj);
					}
			}
			else
			{
				m_SelectedGameObjects->Clear();
				for each(GameObject^ gObj in tempSelectedGameObjects)
					m_SelectedGameObjects->Add(gObj);
			}
		}
		finally {
			Monitor::Exit(m_lock);
		}
		thomas::editor::EditorCamera::Instance()->SetHasSelectionChanged(false);
	}

	System::Guid ThomasSelection::GetSelectedGUID()
	{
		if (m_SelectedGameObjects->Count > 0)
			return m_SelectedGameObjects[0]->m_guid;
		else
			return System::Guid::Empty;
	}

	bool ThomasSelection::Contain(GameObject ^ gObj)
	{
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		bool c;
		try {
			c = m_SelectedGameObjects->Contains(gObj);
		}
		finally{
			Monitor::Exit(m_lock);
		}
		return c;
	}

	GameObject ^ ThomasSelection::operator[](int index)
	{
		Monitor::Enter(m_lock);
#if _DEBUG
		lockOwner = Thread::CurrentThread->Name;
#endif
		GameObject ^ v;
		try {
			v = m_SelectedGameObjects[index];
		}
		finally{
			Monitor::Exit(m_lock);
		}
		return v;
	}


}
