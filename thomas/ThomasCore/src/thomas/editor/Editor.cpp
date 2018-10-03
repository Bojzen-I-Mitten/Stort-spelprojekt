#include "Editor.h"
#include "EditorCamera.h"

namespace thomas 
{
	namespace editor 
	{
		Editor& Editor::GetEditor()
		{
			static Editor editor;
			return editor;
		}

		void Editor::Init()
		{
			// Not currently called
		}

		void Editor::OnEditorPlay()
		{
			ClearEditorState();
		}

		void Editor::OnEditorStop()
		{
		}

		EditorCamera* Editor::Camera()
		{
			// Editor should own the camera instance
			return EditorCamera::Instance();
		}

		void Editor::ClearEditorState()
		{
			EditorCamera::Instance()->UnselectObjects();
		}

	}
}