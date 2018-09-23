#pragma once

#pragma unmanaged


#pragma managed

#using "PresentationFramework.dll"
using namespace System::Runtime::Serialization;

namespace ThomasEngine {

	ref class GameObject;
	[DataContract]
	public ref class Scene
	{
		bool m_playing;
		static bool s_loading = false;
		static Scene^ s_currentScene;
		System::Object^ m_gameObjectsLock = gcnew System::Object();
		System::Collections::ObjectModel::ObservableCollection<GameObject^>^ m_gameObjects = gcnew System::Collections::ObjectModel::ObservableCollection<GameObject^>();
		System::String^ m_name;
		System::String^ m_relativeSavePath;

		Scene();

	internal:
		static bool savingEnabled = true;

	public:
		
		delegate void CurrentSceneChanged(Scene^ newScene);
		static event CurrentSceneChanged^ OnCurrentSceneChanged;


		Scene(System::String^ name);
		void Play();
		void Stop() { m_playing = false; }

		bool IsPlaying() { return m_playing; }

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

		static void SaveSceneAs(Scene^ scene, System::String^ fullPath);
		static void SaveScene(Scene^ scene);

		static Scene^ LoadScene(System::String^ fullPath);
		
		static bool IsLoading()
		{
			return s_loading;
		}

		static void RestartCurrentScene();


		void UnLoad();
		void EnsureLoad();
		void PostLoad();

		static property Scene^ CurrentScene {
			Scene^ get();
			void set(Scene^ value);
		}

	public:
		ref class SceneSurrogate : System::Runtime::Serialization::IDataContractSurrogate
		{
			public:
				virtual System::Type ^ GetDataContractType(System::Type ^type);
				virtual System::Object ^ GetObjectToSerialize(System::Object ^obj, System::Type ^targetType);
				virtual System::Object ^ GetDeserializedObject(System::Object ^obj, System::Type ^targetType);
				virtual System::Object ^ GetCustomDataToExport(System::Reflection::MemberInfo ^memberInfo, System::Type ^dataContractType);
				virtual System::Object ^ GetCustomDataToExport(System::Type ^clrType, System::Type ^dataContractType);
				virtual void GetKnownCustomDataTypes(System::Collections::ObjectModel::Collection<System::Type ^> ^customDataTypes);
				virtual System::Type ^ GetReferencedTypeOnImport(System::String ^typeName, System::String ^typeNamespace, System::Object ^customData);
				virtual System::CodeDom::CodeTypeDeclaration ^ ProcessImportedType(System::CodeDom::CodeTypeDeclaration ^typeDeclaration, System::CodeDom::CodeCompileUnit ^compileUnit);
		};

		
	};
	[System::Runtime::Serialization::DataContractAttribute]
	ref class SceneResource
	{
	public:
		[System::Runtime::Serialization::DataMemberAttribute]
		System::String^ path;
		SceneResource(System::String^ resourcePath) { path = resourcePath; }
	};
}
