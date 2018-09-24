#pragma unmanaged
#include <thomas\object\component\Component.h>
#include <thomas\object\GameObject.h>
#pragma managed

#include "Component.h"
#include "GameObject.h"
#include "../ScriptingManager.h"
using namespace System::Threading;
#include "YieldInstructions.h"

namespace ThomasEngine 
{
	Component::Component() : Object(new thomas::object::component::Component())
	{

	}
	Component::Component(thomas::object::component::Component * ptr) : Object(ptr)
	{
	}

	void Component::Awake() { ((thomas::object::component::Component*)nativePtr)->Awake(); }
	void Component::Start() {}
	void Component::OnEnable() { ((thomas::object::component::Component*)nativePtr)->OnEnable(); }
	void Component::OnDisable() { ((thomas::object::component::Component*)nativePtr)->OnDisable(); };
	void Component::Update() { ((thomas::object::component::Component*)nativePtr)->Update(); }
	void Component::FixedUpdate() { ((thomas::object::component::Component*)nativePtr)->FixedUpdate(); }
	void Component::OnDrawGizmosSelected() { ((thomas::object::component::Component*)nativePtr)->OnDrawGizmosSelected(); }
	void Component::OnDrawGizmos() { ((thomas::object::component::Component*)nativePtr)->OnDrawGizmos(); }

	bool Component::initialized::get() { return ((thomas::object::component::Component*)nativePtr)->initialized; }
	void Component::initialized::set(bool value) { ((thomas::object::component::Component*)nativePtr)->initialized = value; }

	void Component::Initialize() {
		if (!awakened)
		{
			Awake();
			awakened = true;
			return;
		}
		else if (!enabled) {
			enabled = true;
			return;
		}
		else {
			Start();
			initialized = true;
			return;
		}

	}

	void Component::LoadExternalComponents()
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

	
	Transform^ ThomasEngine::Component::transform::get()
	{
		return gameObject->transform;
	}

	void Component::setGameObject(GameObject ^ gObj)
	{
		m_gameObject = gObj;

		((thomas::object::component::Component*)nativePtr)->m_gameObject = (thomas::object::GameObject*)gObj->nativePtr;
		((thomas::object::GameObject*)m_gameObject->nativePtr)->m_components.push_back(((thomas::object::component::Component*)nativePtr));
	}

	void Component::OnCollisionEnter(GameObject ^ collider)
	{
		((thomas::object::component::Component*)nativePtr)->OnCollisionEnter((thomas::object::GameObject*)collider->nativePtr);
	}

	void Component::Destroy()
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
		StopAllCoroutines();
		m_gameObject->Components->Remove(this);
		Object::Destroy();
		Monitor::Exit(m_gameObject->m_componentsLock);
	}

	List<Type^>^ Component::GetAllComponentTypes()
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

	String^ Component::Name::get() {
		return gameObject->Name + " (" + this->GetType()->Name + ")";
	}

	List<Type^>^ Component::GetAllAddableComponentTypes()
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


	void Component::UpdateCoroutines()
	{
		for (int i = 0; i < coroutines->Count; i++)
		{
			System::Collections::IEnumerator^ iterator = coroutines[i];
			bool isYieldInstruction = iterator->Current && YieldInstruction::typeid->IsAssignableFrom(iterator->Current->GetType());
			if (isYieldInstruction && ((YieldInstruction^)iterator->Current)->keepWaiting)
				continue;
			if (!iterator->MoveNext())
			{
				coroutines->RemoveAt(i);
				i--;
			}
		}
	}

	void Component::StartCoroutine(System::Collections::IEnumerator ^ routine)
	{
		coroutines->Add(routine);
	}

	void Component::StopCoroutine(System::Collections::IEnumerator ^ routine)
	{
		for (int i = 0; i < coroutines->Count; i++)
		{
			if (coroutines[i] == routine) {
				coroutines->RemoveAt(i);
				return;
			}

		}
	}

	void Component::StopAllCoroutines()
	{
		coroutines->Clear();
	}
}