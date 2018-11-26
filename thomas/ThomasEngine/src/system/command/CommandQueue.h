#pragma once

#pragma managed
#include "ICommand.h"
using namespace System::Collections::Generic;

namespace ThomasEngine
{
	/* Synced command queue 
	*/
	ref class CommandQueue
	{
	public:

		CommandQueue();

		/* Execute all current commands
		*/
		void execute();
		/* Issue command to be executed later
		*/
		void issue(ICommand^cmd);

	private:
		Object^ m_accessLock;
		/* Input and receive lists
		*/
		List<ICommand^>^ m_commandInput;
		List<ICommand^>^ m_executeList;	// Backbuffer list
	};

}
