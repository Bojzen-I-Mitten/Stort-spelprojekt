#include "Component.h"
#include "GameObject.h"
#include <thomas\object\GameObject.h>
#include "../ScriptingManager.h"

ThomasEngine::Component::Component() : Object(new thomas::object::component::Component())
{
	setGameObject(GameObject::s_lastObject);

}


void ThomasEngine::Component::LoadExternalComponents()
{
	array<String^>^ dlls = Directory::GetFiles(Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location), "Thomas*.dll", SearchOption::TopDirectoryOnly);

	List<System::Type^>^ types = gcnew List<System::Type^>();
	for (int i = 0; i < dlls->Length; i++) {
		Assembly^ assembly = Assembly::LoadFrom(dlls[i]);
		List<System::Type^>^ exportedTypes = gcnew List<System::Type^>(assembly->GetExportedTypes());
		types->AddRange(exportedTypes);
	}
	
	for (int i = 0; i < types->Count; i++)
	{
		if (!Component::typeid->IsAssignableFrom(types[i]))
		{
			types->RemoveAt(i);
			i--;
		}
	}

	externalTypes = types;

}

ThomasEngine::Component::Component(thomas::object::component::Component * ptr) : Object(ptr)
{
	if(GameObject::s_lastObject)
		setGameObject(GameObject::s_lastObject);
}

ThomasEngine::Transform^ ThomasEngine::Component::transform::get()
{
	return gameObject->transform;
}

void ThomasEngine::Component::setGameObject(GameObject ^ gObj)
{
	m_gameObject = gObj;
	
	((thomas::object::component::Component*)nativePtr)->m_gameObject = (thomas::object::GameObject*)gObj->nativePtr;
	((thomas::object::GameObject*)m_gameObject->nativePtr)->m_components.push_back(((thomas::object::component::Component*)nativePtr));
}

void ThomasEngine::Component::Destroy()
{
	Monitor::Enter(m_gameObject->m_componentsLock);
	this->enabled = false;
	for (int i = 0; i < ((thomas::object::GameObject*)m_gameObject->nativePtr)->m_components.size(); i++)
	{
		auto component = ((thomas::object::GameObject*)m_gameObject->nativePtr)->m_components[i];
		if (component == nativePtr)
		{
			((thomas::object::GameObject*)m_gameObject->nativePtr)->
				m_components.erase(((thomas::object::GameObject*)m_gameObject->nativePtr)->m_components.begin() + i);
			break;
		}
	}
	

	m_gameObject->Components->Remove(this);
	Object::Destroy();
	Monitor::Exit(m_gameObject->m_componentsLock);
}

List<Type^>^ ThomasEngine::Component::GetAllComponentTypes()
{
	List<System::Type^>^ types = gcnew List<System::Type^>(externalTypes);

	Assembly^ scriptAssembly = ScriptingManger::GetAssembly();
	if (scriptAssembly)
		types->AddRange(scriptAssembly->GetExportedTypes());

	if (editorAssembly)
		types->AddRange(editorAssembly->GetExportedTypes());

	for (int i = 0; i < types->Count; i++)
	{
		if (!Component::typeid->IsAssignableFrom(types[i]))
		{
			types->RemoveAt(i);
			i--;
		}
	}
	return types;
}

String^ ThomasEngine::Component::Name::get() {
	return gameObject->Name + " (" + this->GetType()->Name + ")";
}

List<Type^>^ ThomasEngine::Component::GetAllAddableComponentTypes()
{
	List<System::Type^>^ types = GetAllComponentTypes();

	for (int i = 0; i < types->Count; i++)
	{
		Type^ t = types[i];
		if (t->IsDefined(HideInInspector::typeid, false))
		{
			types->RemoveAt(i);
			i--;
		}
	}
	return types;
}
