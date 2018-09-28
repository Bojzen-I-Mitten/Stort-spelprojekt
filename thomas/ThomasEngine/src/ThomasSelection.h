#pragma once

#pragma managed

using namespace System::Collections::ObjectModel;
namespace ThomasEngine {

	ref class GameObject;
	public ref class ThomasSelection {
	private:
		ObservableCollection<GameObject^>^ m_SelectedGameObjects;
		Object^ m_lock;
#if _DEBUG
		System::String^ lockOwner;
#endif
	public:

		ThomasSelection();
		~ThomasSelection();

		void SelectGameObject(GameObject^ gObj);
		void SelectGameObject(System::Guid guid);
		void UnSelectGameObject(GameObject^ gObj);

		void UnselectGameObjects();

		void render();

		void UpdateSelectedObjects();

		System::Guid GetSelectedGUID();
		

		bool Contain(GameObject^ gObj);

	public:
		GameObject^ operator[](int index);

		property int Count {
			int get() { return m_SelectedGameObjects->Count; }
		}

		property const ObservableCollection<GameObject^>^ Ref {
			const ObservableCollection<GameObject^>^ get() { return m_SelectedGameObjects; }
		}
	};
}
