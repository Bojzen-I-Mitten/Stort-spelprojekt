#pragma once

#pragma unmanaged

#include <stdint.h>
#include <thomas/ThomasCore.h>
#include <thomas/object/GameObject.h>

#pragma managed

#using "PresentationFramework.dll"
using namespace System::Runtime::Serialization;
using namespace System::Collections::Generic;

namespace ThomasEngine {

	//Forw. declare
	ref class GameObject;
	ref class Component;

	/* Scene in the system. Responsible for containing active objects (and by inheritance their components)
	*/
	[DataContract]
	public ref class Scene
	{
	public:
		ref class SceneObjectsChangedArgs : System::EventArgs
		{
		public:
#ifdef _EDITOR
			const uint32_t EditCount;			// Number of objects edited
			const List<GameObject^>^ Added;		// List of objects added
			const List<GameObject^>^ Removed;	// List of objects removed
			SceneObjectsChangedArgs(uint32_t EditCount, List<GameObject^>^ add, List<GameObject^>^ rmv)
				: EditCount(EditCount), Added(add), Removed(rmv)
			{			}
		};
		delegate void SceneObjectsChangedEventHandler(System::Object^ sender, SceneObjectsChangedArgs^ e);
#endif
	private:
		enum class Command
		{
			Add,			// Add object
			Remove,			// Stage 2. Remove object
			DisableRemove	// Stage 1. Disable removed object, added as following command: Remove
		};
		value struct IssuedCommand
		{
			Command m_cmd;
			GameObject^ m_obj;
		};

		uint32_t m_uniqueID;
		List<GameObject^>^ m_gameObjects = gcnew List<GameObject^>();
		System::Object^ m_accessLock = gcnew System::Object();
		List<IssuedCommand>^ m_commandList;
		List<IssuedCommand>^ m_commandSwapList;
		System::String^ m_name;
		System::String^ m_relativeSavePath;

#ifdef _EDITOR
		event SceneObjectsChangedEventHandler^ m_changeEvent;
#endif

	private:
		Scene(uint32_t unique_id);

		/* Instantly remove and delete an object.
		*/
		void DeleteInstant(GameObject ^ object);

		/* Verify object is in scene
		*/
		bool InScene(GameObject ^ gObj, int &index);

	public:
		static Scene^ LoadScene(System::String^ fullPath, uint32_t unique_id);
		/* Verify object is in scene
		*/
		bool InScene(GameObject ^ gObj);
	public:

		Scene(System::String^ name, uint32_t unique_id);
		~Scene();

		void AwakeObjects(List<GameObject^>^ objects, bool playing);

		void EnableObjects(List<GameObject^>^ objects, bool playing);

		uint32_t ID() { return m_uniqueID; } // 0 is reserved, unique ID's to be implemented.

		bool OnPlay();

		/* Schedule an object to be created during synchronized state
		*/
		void CreateObject(GameObject^ object);
		/* Schedule destruction of an object during synchonized state
		*/
		void DestroyObject(GameObject^ object);

		/* Find gameobject by name
		*/
		GameObject^ Find(System::String^ name);
		/* Find gameobject by GUID
		*/
		GameObject^ Find(System::Guid guid);
		/* Find gameobject by it's native pointer
		*/
		GameObject^ Find(thomas::object::GameObject* native);

		generic<typename T>
			where T : Component
		List<T>^ getComponentsOfType();
		List<System::Object^>^ getComponentsOfType(System::Type^ type);
		/* Find the first component available.
		*/
		generic<typename T>
			where T : Component
		T findFirstComponent();

#pragma region Serialized properties

		[DataMember(Order = 1)]
		property System::String^ Name {
			System::String^ get() { return m_name; }
			void set(System::String^ value) { m_name = value; }
		}

		[DataMember(Order = 2)]
		property System::String^ RelativeSavePath {
			System::String^ get() { return m_relativeSavePath; }
			void set(System::String^ value) { m_relativeSavePath = value; }
		}
	private:

		[DataMember(Order = 3)]
		property Vector3 CameraPosition {
			Vector3 get();
			void set(Vector3);
		}

		[DataMember(Order = 4)]
		property Vector3 CameraEuler{
			Vector3 get();
			void set(Vector3);
		}
		/* DataContract serialization game object list
		*/
		[DataMember(Order = 5)]
		property List<GameObject^>^ GameObjectData {
			List<GameObject^>^ get();
			void set(List<GameObject^>^ val);
		}


#pragma endregion
	
	public:


		void SaveSceneAs(System::String^ fullPath);
		void SaveScene();
		

#ifdef _EDITOR
		property IEnumerable<GameObject^>^ GameObjectsSynced { IEnumerable<GameObject^>^ get(); }
		void Subscribe(SceneObjectsChangedEventHandler^ func);
		void Unsubscribe(SceneObjectsChangedEventHandler^ func);
#endif
	internal:
		void UnLoad();
		void EnsureLoad();
		void PostLoad();
		void SyncScene();

		/* Engine access to objects. Engine edits are always in sync.
		*/
		property IEnumerable<GameObject^>^ GameObjects
		{
			/* Get all objects in the scene
			*/
			System::Collections::Generic::IEnumerable<GameObject^>^ get();
		}
	};
}

