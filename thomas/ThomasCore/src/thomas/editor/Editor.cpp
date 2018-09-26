#include "Editor.h"
#include "EditorCamera.h"

namespace thomas {
	namespace editor {



		Editor & Editor::GetEditor()
		{
			/* Singleton*/
			static Editor SYS_EDITOR;
			return SYS_EDITOR;
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

		EditorCamera * Editor::Camera()
		{
			return Editor::Camera();
		}

		void Editor::ClearEditorState()
		{
			EditorCamera::GetEditorCamera()->UnselectObjects();
		}

	}
}