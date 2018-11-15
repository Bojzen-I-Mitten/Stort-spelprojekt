#pragma once
#pragma unmanaged 

#include <thomas/object/GameObject.h>
#include "ComponentState.h"
#pragma managed
#include "../attributes/CustomAttributes.h"
#include "Object.h"
using namespace System::Collections::ObjectModel;

namespace ThomasEngine 
{
	enum class PrimitiveType;
	ref class Scene;
	ref class Transform;
	ref class Component;

	public ref class GameObject : public Object
	{
	private:
		
		ObservableCollection<Component^> m_components;
		Transform^ m_transform;
		uint32_t m_scene_id;
		bool m_makeDynamic = false;
		bool m_makeStatic = false;

	private:
		GameObject();
		virtual ~GameObject();

		System::Object^ m_componentsLock = gcnew System::Object();
		
	internal:
		/* Init the components within the object to the specified state
		s		<<	State components should be initiated to
		playing	<<	If components are 'running'
		*/
		void InitComponents(Comp::State s, bool playing);

		static void FlattenGameObjectTree(List<GameObject^>^ list, GameObject ^ root);

		bool RemoveComponent(Component^ comp);
		virtual void Destroy() override;

		System::String^ prefabPath;


		void SyncComponents();

		void PostLoad(Scene^ scene);
		/* Post instantiate the object, adding it to the scene etc.
		*/
		void PostInstantiate(Scene^ scene);
		/* Clean out null components (no lock applied, assert stable state)
		*/
		void CleanComponents();

		
		thomas::object::Object* setStatic();
		thomas::object::Object* moveStaticGroup();
		thomas::object::Object* setDynamic();
				
		void Update();

		void FixedUpdate();

		void RenderGizmos();

		void RenderSelectedGizmos();
		
		

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property thomas::object::GameObject* Native {
			thomas::object::GameObject* get();
		}

	public:

		bool IsPrefab();

		bool MakeStatic();
		bool MakeDynamic();
		bool MoveStaticGroup();
		void OnDestroy() override;

		GameObject(String^ name);

		void DestroySelf();

		property bool inScene {
			bool get() {
				return m_scene_id; // != 0
			}
		}
		property bool activeSelf
		{
			bool get();
			void set(bool value);
		}	

		property UINT GroupIDSelf
		{
			UINT get();
			void set(UINT state);
		}

		property bool staticSelf
		{
			bool get();
			void set(bool state);
		}

		[BrowsableAttribute(false)]
		property String^ Name
		{
			String^ get() override;
			void set(String^) override;
		};

		property String^ Tag;
		property int Layer
		{
			int get();
			void set(int value);
		}

		String^ ToString() override
		{
			return Name;
		}


		[BrowsableAttribute(false)]
		[Newtonsoft::Json::JsonIgnoreAttribute]
		property Transform^ transform 
		{
			Transform^ get();

			void set(Transform^ value);
		}

		property ObservableCollection<Component^>^ Components 
		{
			ObservableCollection<Component^>^ get() 
			{
				return %m_components;
			}
			void set(ObservableCollection<Component^>^ value)
			{
				m_components.Clear();
				for each(Component^ c in value)
					m_components.Add(c);
			}
		}

		generic<typename T>
		where T : Component
		T AddComponent();

		generic<typename T>
		where T : Component
		T GetComponent();

		/* Find component from it's native pointer. Returns the component casted to the type or null if not found or cast failed.
		*/
		generic<typename T>
		where T : Component
		T GetComponent(void* nativePointer);

		Component^ GetComponent(Type^ type);


		generic<typename T>
		where T : Component
		List<T>^ GetComponents();


		List<Component^>^ GetComponents(Type^ type);


		bool HasComponentOfType(Type^ T);


		static GameObject^ Find(String^ name);

		static GameObject^ CreatePrimitive(PrimitiveType type);

		bool GetActive();
		void SetActive(bool active);

		static GameObject^ Instantiate(GameObject^ original);
		static GameObject^ Instantiate(GameObject^ original, Transform^ parent);
		static GameObject^ Instantiate(GameObject^ original, Vector3 position, Quaternion rotation);
		static GameObject^ Instantiate(GameObject^ original, Vector3 position, Quaternion rotation, Transform^ parent);
		/* Create an empty GameObject 'prefab' used from editor to create a new prefab object */
		static GameObject^ CreateEmptyPrefab();



		[System::Runtime::Serialization::OnDeserializedAttribute]
		void OnDeserialized(System::Runtime::Serialization::StreamingContext c);
	};
}