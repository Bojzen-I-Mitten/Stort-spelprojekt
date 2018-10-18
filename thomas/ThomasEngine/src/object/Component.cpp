#pragma unmanaged
#include <thomas\object\component\Component.h>
#include <thomas\object\GameObject.h>
#pragma managed

#include "Component.h"
#include "GameObject.h"
#include "../ScriptingManager.h"
using namespace System::Threading;
#include "YieldInstructions.h"
#include "../Debug.h"

namespace ThomasEngine 
{
	Component::Component() : Object(new thomas::object::component::Component())
	{

	}
	Component::Component(thomas::object::component::Component * ptr) : Object(ptr)
	{
	}
	Component::~Component()
	{
		Delete();
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
			initialized = true;
			return;
		}
		else if (!m_firstEnable && enabled)
		{
			m_firstEnable = true;
			OnEnable();
			return;
		}
		else if(!m_started) {
			m_started = true;
			Start();
			initialized = true;
			return;
		}
		initialized = true;
	}

	bool Component::enabled::get() { return m_enabled; }
	void Component::enabled::set(bool value) {
		if (m_enabled != value) {
			m_enabled = value;
			if (m_firstEnable && m_gameObject->GetActive()) {
				if (value == true) {
					Enable();
					initialized = false;
				}
				else
					Disable();
			}else
				initialized = false;
		}
	}

	void Component::Disable()
	{
		m_enabled = false;
		OnDisable();
	}
	void Component::Enable()
	{
		m_enabled = true;
		OnEnable();
	}

	void Component::LoadExternalComponents()
	{
		List<String^>^ dlls = gcnew List<String^>(Directory::GetFiles(Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location), "Thomas*.dll", SearchOption::TopDirectoryOnly));

		String^ assemblyPath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location) + "\\Assembly.dll";
		if (File::Exists(assemblyPath))
			dlls->Add(assemblyPath);

		List<System::Type^>^ types = gcnew List<System::Type^>();
		for (int i = 0; i < dlls->Count; i++) {
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
		OnGameObjectSet();
	}


	void Component::Destroy()
	{
		Monitor::Enter(m_gameObject->m_componentsLock);
		m_gameObject->Components->Remove(this);
		Delete();
		Monitor::Exit(m_gameObject->m_componentsLock);
		// Destroy the object
		Object::Destroy();
	}

	void Component::Delete()
	{
		Disable();	// Disable first just in case
#ifdef _DEBUG
		// Check successfull destruction
		if(m_gameObject->Native->RemoveComponent(this->nativePtr))
			Debug::LogWarning("Component destruction failed in object: " + m_gameObject->Name + ". Component of type: " + this->GetType());
#else
		// Don't care
		m_gameObject->Native->RemoveComponent(this->nativePtr);
#endif
		StopAllCoroutines();
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