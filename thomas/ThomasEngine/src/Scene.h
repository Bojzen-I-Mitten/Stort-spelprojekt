#pragma once

#pragma managed
namespace ThomasEngine {
	ref class GameObject;
	public ref class Scene
	{
		bool m_playing;
		static bool s_loading = false;
		static Scene^ s_currentScene;
		System::Object^ m_gameObjectsLock = gcnew System::Object();
		System::Collections::ObjectModel::ObservableCollection<GameObject^> m_gameObjects;
		System::String^ m_name;
		System::String^ m_relativeSavePath;

		Scene();

	internal:
		static bool savingEnabled = true;

	public:

		static System::Collections::Specialized::NotifyCollectionChangedEventHandler^ sceneChanged;

		Scene(System::String^ name);
		void Play();
		void Stop() { m_playing = false; }

		bool IsPlaying() { return m_playing; }

		property System::String^ RelativeSavePath {
			System::String^ get() { return m_relativeSavePath; }
			void set(System::String^ value) { m_relativeSavePath = value; }
		}

		System::Object^ GetGameObjectsLock()
		{
			return m_gameObjectsLock;
		}

		property System::String^ Name {
			System::String^ get() { return m_name; }
			void set(System::String^ value) { m_name = value; }
		}


		property System::Collections::ObjectModel::ObservableCollection<GameObject^>^ GameObjects {
			System::Collections::ObjectModel::ObservableCollection<GameObject^>^ get() {
				return %m_gameObjects;
			}
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
		void PostLoad();

		static property Scene^ CurrentScene {
			Scene^ get();
			void set(Scene^ value);
		}
	};
}
