#pragma unmanaged
#include <thomas\object\component\Component.h>
#include <thomas\object\GameObject.h>
#include <thomas\utils\Utility.h>
#pragma managed

#include "Component.h"
#include "GameObject.h"
#include "../ScriptingManager.h"
using namespace System::Threading;
#include "YieldInstructions.h"
#include "../Debug.h"
#include "component/Transform.h"

namespace ThomasEngine 
{
	Component::Component() : 
		Component(new thomas::object::component::Component())
	{

	}
	Component::Component(thomas::object::component::Component * ptr) 
		: Object(ptr), m_state(Comp::State::Uninitialized),
		m_active(true)
	{
	}
	Component::~Component()
	{
		//Disable();	
#ifdef _DEBUG
		// Check successfull destruction
		if (m_gameObject->Native->RemoveComponent(this->nativePtr))
			Debug::LogWarning("Component destruction failed in object: " + m_gameObject->Name + ". Component of type: " + this->GetType());
#else
		// Don't care
		m_gameObject->Native->RemoveComponent(this->nativePtr);
#endif
		StopAllCoroutines();
	}

	void Component::Destroy()
	{
		gameObject->RemoveComponent(this);
	}



	void Component::OnAwake() {	}
	//void Component::Start() {}
	void Component::OnEnable() { 	}
	void Component::OnDisable() {	}
	void Component::Start() { }
	void Component::Update() { ((thomas::object::component::Component*)nativePtr)->Update(); }
	void Component::FixedUpdate() { ((thomas::object::component::Component*)nativePtr)->FixedUpdate(); }
	void Component::OnDrawGizmosSelected() { ((thomas::object::component::Component*)nativePtr)->OnDrawGizmosSelected(); }
	void Component::OnDrawGizmos() { ((thomas::object::component::Component*)nativePtr)->OnDrawGizmos(); }
	void Component::OnDestroy() {/* ((thomas::object::component::Component*)nativePtr)->OnDestroy(); */}

	bool Component::enableInEditor()
	{
		return GetType()->IsDefined(ExecuteInEditor::typeid, false);
	}


	bool Component::enabled::get() { return m_state == Comp::State::Enabled; }
	void Component::enabled::set(bool value)
	{
		if (!m_gameObject->GetActive()) return;
		if (value)
		{
			//	Enable
			if (m_state == Comp::State::Disabled || m_state == Comp::State::Awake)
				Enable();
		}
		else if (m_state == Comp::State::Enabled)
		{
			//	Disable
			Disable();
		}
		else if (m_state == Comp::State::Awake)
			m_active = false;	// Ensure object activation isn't run.

	}
	/* Initiation of a single component. Ensure GameObject is Active
	*/
	void Component::InitComponent(Comp::State s, uint32_t InitBits)
	{
		using namespace thomas;
		/* Object initiation Process.
		 * 1. Awake()
		 * 2. if 'Active'
			Enable(), Start()

		 * Editor is a separate case where not all objects are enabled
		*/
		switch (s)
		{
		case Comp::State::Awake:
		{
#ifdef _EDITOR														
			// If editor state: don't initiate all components
			if (!(utility::hasFlag(InitBits, INIT_PLAYING_BIT) || this->enableInEditor())) return;
			// Ensure component isn't initialized
			if (this->ComponentState != Comp::State::Uninitialized) return;
#else		
			assert(c->State == Comp::State::Uninitialized);
#endif			
			this->Awake();
		}
		break;
		case Comp::State::Enabled:
		{
			// If Component isn't activated, and not explicit call: ignore
			// If Component isn't in a valid state for this call:	ignore
			if ((!this->Activated && !utility::hasFlag(InitBits, INIT_EXPLICIT_CALL_BIT)) ||
				!(this->ComponentState == Comp::State::Awake || this->ComponentState == Comp::State::Disabled))
				return;
			this->Enable();
		}
		break;
		case Comp::State::Disabled:
		{
			if (!this->Activated)
				return;	// Ignore inactive components.

			if (this->ComponentState == Comp::State::Enabled)
				this->Disable();
		}
		break;
		case Comp::State::Uninitialized:
		case Comp::State::EndState:
		default:
			assert(false); // Don't
			break;
		}
	}

	void Component::Awake()
	{
		m_state = Comp::State::Awake;
		OnAwake();
		((thomas::object::component::Component*)nativePtr)->OnAwake();
	}
	void Component::Enable()
	{
		bool runStart = m_state == Comp::State::Awake;
		// Enable engine component, then change Core state
		m_state = Comp::State::Enabled;
		OnEnable();
		((thomas::object::component::Component*)nativePtr)->OnEnable();
		((thomas::object::component::Component*)nativePtr)->setEnabledState(true);

		// Start!
		if (runStart)
			Start();
	}

	void Component::Disable()
	{
		if (m_state == Comp::State::Awake) return;
		// Disable engine component, then change Core state
		m_state = Comp::State::Disabled;
		OnDisable();
		((thomas::object::component::Component*)nativePtr)->OnDisable();
		((thomas::object::component::Component*)nativePtr)->setEnabledState(false);
	}

	void Component::LoadExternalComponents()
	{
		List<String^>^ dlls = gcnew List<String^>(Directory::GetFiles(Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location), "Thomas*.dll", SearchOption::TopDirectoryOnly));

		String^ assemblyPath = Path::GetDirectoryName(Assembly::GetExecutingAssembly()->Location) + "\\Assembly.dll";
		if (File::Exists(assemblyPath))
			dlls->Add(assemblyPath);

		List<System::Type^>^ types = gcnew List<System::Type^>();
		for (int i = 0; i < dlls->Count; i++) {
			Assembly^ assembly = Assembly::UnsafeLoadFrom(dlls[i]);
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
		((thomas::object::GameObject*)m_gameObject->nativePtr)->appendComponent(((thomas::object::component::Component*)nativePtr));
		OnGameObjectSet();
	}

	List<Type^>^ Component::GetAllComponentTypes()
	{
		List<System::Type^>^ types = gcnew List<System::Type^>(externalTypes);

		Assembly^ scriptAssembly = ScriptingManager::GetAssembly();
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
	bool Component::Activated::get()
	{
		return m_active;
	}
	void Component::Activated::set(bool state)
	{
		if (dynamic_cast<Transform^>(this) != nullptr) 
			m_active = true; // Can't be deactivated
		else
			m_active = state;
	}
	Comp::State Component::ComponentState::get()
	{ 
		return m_state; 
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
				if(coroutines->Count > i)
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
