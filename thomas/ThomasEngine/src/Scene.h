#pragma once

#pragma unmanaged

#include <stdint.h>

#pragma managed

#using "PresentationFramework.dll"
using namespace System::Runtime::Serialization;

namespace ThomasEngine {
	
	ref class GameObject;
	[DataContract]
	public ref class Scene
	{
	private:

		uint32_t m_uniqueID;
		System::Object^ m_gameObjectsLock = gcnew System::Object();
		System::Collections::ObjectModel::ObservableCollection<GameObject^>^ m_gameObjects = gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>();
		System::String^ m_name;
		System::String^ m_relativeSavePath;

		Scene(uint32_t unique_id);

	public:
		static Scene^ LoadScene(System::String^ fullPath, uint32_t unique_id);
	public:

		Scene(System::String^ name, uint32_t unique_id);
		~Scene();

		uint32_t ID() { return m_uniqueID; } // 0 is reserved, unique ID's to be implemented.

		void OnPlay();


		void InitGameObjects(bool playing);

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
		/* DataContract serialization access to game object list
		*/
		[DataMember(Order = 5)]
		property System::Collections::ObjectModel::ObservableCollection<GameObject^>^ GameObjectData {
			System::Collections::ObjectModel::ObservableCollection<GameObject^>^ get() {
				return m_gameObjects;
			}
			void set(System::Collections::ObjectModel::ObservableCollection<GameObject^>^ val) {
				m_gameObjects = val;
				m_gameObjectsLock = gcnew Object();
				System::Windows::Data::BindingOperations::EnableCollectionSynchronization(m_gameObjects, m_gameObjectsLock);
			}
		}

	public:
		/* List access to scene game objects
		*/
		[IgnoreDataMemberAttribute]
		property System::Collections::ObjectModel::ObservableCollection<GameObject^>^ GameObjects {
			System::Collections::ObjectModel::ObservableCollection<GameObject^>^ get() {
				return m_gameObjects;
			}
		}

#pragma endregion

		System::Object^ GetGameObjectsLock()
		{
			return m_gameObjectsLock;
		}

		void SaveSceneAs(System::String^ fullPath);
		void SaveScene();



		void UnLoad();
		void EnsureLoad();
		void PostLoad();

	public:
	};
}

