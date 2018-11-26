#pragma once


namespace ThomasEngine
{
	/* Command Interface for messages to execute in engine
	*/ 
	public interface class ICommand
	{
		void execute();
	};

	generic<typename T>
	public ref class Command :
		ICommand
	{
	public:
		delegate void CmdFunc(T data);

		Command(CmdFunc^ cmd, T data);
		// Inherited via ICommand
		virtual void execute();
	private:

		CmdFunc^ m_func;
		T m_payload;
	};

	generic<typename T>
	inline Command<T>::Command(CmdFunc^ cmd, T data)
		: m_func(cmd), m_payload(data)
	{
	}

	generic<typename T>
	inline void Command<T>::execute()
	{
		m_func(m_payload);
	}

}