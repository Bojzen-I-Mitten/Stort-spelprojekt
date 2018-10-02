#pragma once
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

		System::String^ prefabPath;

		bool m_isDestroyed = false;
		System::Object^ m_componentsLock = gcnew System::Object();

		static void SerializeGameObject(String^ path, GameObject^ gObj);
		void SyncComponents();
		static System::IO::Stream^ SerializeGameObject(GameObject^ gObj);
		static GameObject^ DeSerializeGameObject(System::IO::Stream^ stream);

		void PostLoad(Scene^ scene);

		void PostInstantiate(Scene^ scene);


		static void InitGameObjects(bool playing);

		
				
		void Update();

		void FixedUpdate();

		void RenderGizmos();

		void RenderSelectedGizmos();
		
	public:

		GameObject(String^ name);
		
		static GameObject^ CreatePrefab();

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

		String^ ToString() override
		{
			return Name;
		}

		[BrowsableAttribute(false)]
		[Xml::Serialization::XmlIgnoreAttribute]
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