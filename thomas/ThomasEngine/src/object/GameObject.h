#pragma once
#pragma unmanaged 

#include <thomas/object/GameObject.h>
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
		Scene^ scene;

		GameObject();

		bool InitComponents(bool playing);

	internal:

		static void FlattenGameObjectTree(List<GameObject^>^ list, GameObject ^ root);

		System::String^ prefabPath;

		bool m_isDestroyed = false;
		static bool m_toBeStatic = false;
		System::Object^ m_componentsLock = gcnew System::Object();

		void SyncComponents();

		void PostLoad(Scene^ scene);

		void PostInstantiate(Scene^ scene);


		static void InitGameObjects(bool playing);

		void setStatic();

		GameObject^ FindGameObjectFromNativePtr(thomas::object::GameObject* nativeptr);
				
		void Update();

		void FixedUpdate();

		void RenderGizmos();

		void RenderSelectedGizmos();
		

		[Newtonsoft::Json::JsonIgnoreAttribute]
		property thomas::object::GameObject* Native {
			thomas::object::GameObject* get();
		}

	public:
		bool getToBeStatic();
		GameObject(String^ name);
		
		static GameObject^ CreatePrefab();
		static void toBeStatic();
		property bool inScene {
			bool get() {
				return scene != nullptr;
			}
		}

		virtual void Destroy() override;

		property bool activeSelf
		{
			bool get();
			void set(bool value);
		}	

		[BrowsableAttribute(false)]
		property String^ Name
		{
			String^ get() override;
			void set(String^) override;
		};

		

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

		Component^ GetComponent(Type^ type);


		generic<typename T>
		where T : Component
		List<T>^ GetComponents();


		List<Component^>^ GetComponents(Type^ type);


		bool HasComponentOfType(Type^ T);

		static List<GameObject^>^ GetAllGameObjects(bool includePrefabs);

		static GameObject^ Find(String^ name);

		static GameObject^ CreatePrimitive(PrimitiveType type);

		bool GetActive();

		void SetActive(bool active);

		static GameObject^ Instantiate(GameObject^ original);
		static GameObject^ Instantiate(GameObject^ original, Transform^ parent);
		static GameObject^ Instantiate(GameObject^ original, Vector3 position, Quaternion rotation);
		static GameObject^ Instantiate(GameObject^ original, Vector3 position, Quaternion rotation, Transform^ parent);


		[System::Runtime::Serialization::OnDeserializedAttribute]
		void OnDeserialized(System::Runtime::Serialization::StreamingContext c);
	};
}