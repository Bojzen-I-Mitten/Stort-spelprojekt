#pragma once


namespace thomas {
	namespace editor {

		class EditorCamera;
		
		class Editor {
		public:
			static Editor& GetEditor();
		public:

			void Init();

			void OnEditorPlay();
			void OnEditorStop();

			EditorCamera* Camera();

		private:
			/* Function called when editor goes out of context in the system
			 * (such as when game is activated)
			*/
			void ClearEditorState();
		};
	}
}