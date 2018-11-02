#include "CommandQueue.h"


#pragma managed

using namespace System::Threading;

namespace ThomasEngine
{
	CommandQueue::CommandQueue()	:
		m_accessLock(gcnew Object()),
		m_commandInput(gcnew List<ICommand^>()),
		m_executeList(gcnew List<ICommand^>())
	{
	}
	void CommandQueue::execute()
	{
		Monitor::Enter(m_accessLock);
		auto swp = m_commandInput;
		m_commandInput = m_executeList;
		m_executeList = swp;
		Monitor::Exit(m_accessLock);

		for each(auto cmd in m_executeList)
			cmd->execute();
	}
	void CommandQueue::issue(ICommand ^ cmd)
	{
		Monitor::Enter(m_accessLock);
		m_commandInput->Add(cmd);
		Monitor::Exit(m_accessLock);
	}
}