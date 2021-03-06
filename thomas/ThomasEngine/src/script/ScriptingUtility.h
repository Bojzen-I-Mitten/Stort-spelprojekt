#pragma once


#pragma managed
using namespace System::Collections::Generic;


namespace ThomasEngine
{
	ref class GameObject;
	ref class Component;
	ref class Material;
	namespace Script
	{
		/* Script Utility Functions, primary use for external access not internal
		*/
		public ref class ScriptUtility
		{
		public:
			/* Find all components of type
			*/
			generic<typename T>
				where T : Component
			static IEnumerable<T>^ GetComponentsOfType();

			/* Find the first component of type, otherwise default value T() is returned.
			*/
			generic<typename T>
				where T : Component
			static T FindComponent();
		};
	}
}