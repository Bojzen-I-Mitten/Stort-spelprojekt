/*
	Singeleton class for management of the editor camera in the scene. 
 */

#pragma once
#include <imgui\ImGuizmo.h>
#include <vector>
#include <memory>
#include "..\object\GameObject.h"
namespace thomas
{
	namespace resource
	{
		class Material;
	}
	namespace object 
	{
		namespace component 
		{
			class Camera;
		}
	}

	namespace editor
	{
		class EditorGrid;

		class EditorCamera : public object::GameObject
		{
		public:		
			void Init();
			void Destroy();
			void Render();
			void Update();

		public:
			bool HasSelectionChanged();
			void ToggleManipulatorMode();
			void ToggleObjectSelection(GameObject* gameObject);
			void SelectObject(GameObject* gameObject);
			void UnselectObject(GameObject* gameObject);
			void UnselectObjects();

		public:
			void SetHasSelectionChanged(const bool & selectionChanged);
			void SetManipulatorOperation(ImGuizmo::OPERATION operation);

		public:
			static EditorCamera* Instance();
			ImGuizmo::OPERATION GetManipulatorOperation();
			const std::vector<object::GameObject*>& GetSelectedObjects();
			object::component::Camera* GetCamera() const;

		private:
			void RenderCamera();
			void UpdateCamera();
			void RenderSelectedObjects();
			void RenderGizmos();
			void MoveAndRotateCamera();
			void SnapCameraToFocus();
			object::GameObject* FindClickedGameObject();
			void BoxSelect();
			EditorCamera();
			~EditorCamera();

		private:
			ImGuizmo::OPERATION m_manipulatorOperation;
			ImGuizmo::MODE m_manipulatorMode;
			std::unique_ptr<object::component::Camera> m_cameraComponent;
			std::unique_ptr<resource::Material> m_objectHighlighter;
			std::unique_ptr<EditorGrid> m_grid;

		private:
			float m_sensitivity;
			float m_speed;
			bool m_manipulatorSnapping;	
			bool m_hasSelectionChanged;
			bool m_isBoxSelecting;
			math::Rectangle m_boxSelectRect;
			object::GameObject* m_selectedObject;
			std::vector<object::GameObject*> m_selectedObjects;

		private:
			static EditorCamera m_editorCamera;
		};
	}
}
