#pragma once

#pragma unmanaged

#include <stdint.h>
#include <thomas/ThomasCore.h>

#pragma managed

#using "PresentationFramework.dll"
using namespace System::Runtime::Serialization;

namespace ThomasEngine {

	
	ref class GameObject;
	[DataContract]
	public ref class Scene
	{
	private:
		enum class Command
		{
			Add,
			Remove
		};
		value struct IssuedCommand
		{
			Command m_cmd;
			GameObject^ m_obj;
		};

		uint32_t m_uniqueID;
		System::Collections::ObjectModel::ObservableCollection<GameObject^>^ m_gameObjects = gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>();
		System::Object^ m_accessLock = gcnew System::Object();
		System::Collections::Generic::List<IssuedCommand>^ m_commandList;
		System::String^ m_name;
		System::String^ m_relativeSavePath;
		//thomas::utils::atomics::SynchronizedList< GameObject^>* m_list;

		Scene(uint32_t unique_id);

	public:
		static Scene^ LoadScene(System::String^ fullPath, uint32_t unique_id);
	public:

		Scene(System::String^ name, uint32_t unique_id);
		~Scene();

		uint32_t ID() { return m_uniqueID; } // 0 is reserved, unique ID's to be implemented.

		void OnPlay();

		void CreateObject(GameObject^ object);
		void DestroyObject(GameObject^ object);

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
		/* DataContract serialization game object list
		*/
		[DataMember(Order = 5)]
		property System::Collections::ObjectModel::ObservableCollection<GameObject^>^ GameObjectData {
			System::Collections::ObjectModel::ObservableCollection<GameObject^>^ get();
			void set(System::Collections::ObjectModel::ObservableCollection<GameObject^>^ val);
		}

	public:

		property System::Collections::Generic::IEnumerable<GameObject^>^ GameObjects
		{
			System::Collections::Generic::IEnumerable<GameObject^>^ get();
		}

#pragma endregion

		void SaveSceneAs(System::String^ fullPath);
		void SaveScene();

		void Add(GameObject^obj);
		void Remove(GameObject^obj);

		void UnLoad();
		void EnsureLoad();
		void PostLoad();

	private:
	};
}

