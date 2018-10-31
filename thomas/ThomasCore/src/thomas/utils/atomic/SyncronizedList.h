#pragma once

#include <vector>
#include "Synchronization.h"
namespace thomas
{
	namespace utils
	{
		namespace atomics
		{
			/* Protected list editing the synchronized values when
			*/
			template<class T>
			class SynchronizedList
			{
			public:
				SynchronizedList();
				~SynchronizedList();
				void add(T& e);
				void rmv(T& e);
				/* Sync. issued edits. Returns number of successfull edits.
				*/
				size_t sync();
				/* Find using a comparator struct:
				* Implement with func:
				bool operator()(T& compared)
				*/
				template<class C>
				bool find(C comp, T& ret);
				/* Find a specific element
				*/
				bool find(T &comp, T& ret);
				size_t size();
				T& operator[](uint32_t index);
				typename std::vector<T>::iterator begin();
				typename std::vector<T>::iterator end();
				typename std::vector<T>::const_iterator begin() const;
				typename std::vector<T>::const_iterator end() const;
			private:
				enum EditCommand
				{
					Remove,
					Add
				};

				std::vector<T> m_syncedList;
				SpinLock m_lock;
				std::vector<std::pair<EditCommand, T>> m_waitingList;
			};
			template<class T>
			SynchronizedList<T>::SynchronizedList() :
				m_syncedList(),
				m_lock(),
				m_waitingList()
			{

			}
			template<class T>
			SynchronizedList<T>::~SynchronizedList()
			{

			}
			template<class T>
			T& SynchronizedList<T>::operator[](uint32_t index)
			{
				return m_syncedList[index];
			}
			template<class T>
			size_t SynchronizedList<T>::size()
			{
				return m_syncedList.size();
			}
			template<class T>
			void SynchronizedList<T>::add(T& e)
			{
				m_lock.lock();
				std::pair<EditCommand, T> p(EditCommand::Add, e);
				m_waitingList.push_back(p);
				m_lock.unlock();
			}
			template<class T>
			void SynchronizedList<T>::rmv(T& e)
			{
				m_lock.lock();
				std::pair<EditCommand, T> p(EditCommand::Remove, e);
				m_waitingList.push_back(p);
				m_lock.unlock();
			}
			template<class T>
			size_t SynchronizedList<T>::sync()
			{
				size_t edits = 0;
				m_lock.lock();	// Lock to assert sync.
				for (std::pair<EditCommand, T> p : m_waitingList)
				{
					switch (p.first)
					{
					case EditCommand::Remove:
					{
						for (auto it = m_syncedList.begin(); it < m_syncedList.end(); it++)
						{
							if (*it == p.second)
							{
								m_syncedList.erase(it);
								edits++;
								break;
							}
						}
					}
					break;
					case EditCommand::Add:
						m_syncedList.push_back(p.second);
						edits++;
						break;
					default:
						// Invalid command
						break;
					}
				}
				m_waitingList.clear();
				m_lock.unlock();
				return edits;
			}

			template<class T>
			template<class C>
			bool SynchronizedList<T>::find(C comp, T& ret)
			{
				for (T& e : m_syncedList)
				{
					if (comp(e))
					{
						ret = e;
						return true;
					}
				}
				return false;
			}
			template<class T>
			bool SynchronizedList<T>::find(T& comp, T& ret)
			{
				for (T& e : m_syncedList)
				{
					if (e == comp)
					{
						ret = e;
						return true;
					}
				}
				return nullptr;
			}

			template<class T>
			typename std::vector<T>::iterator SynchronizedList<T>::begin()
			{
				return m_syncedList.begin();
			}
			template<class T>
			typename std::vector<T>::iterator SynchronizedList<T>::end()
			{
				return m_syncedList.end();
			}

			template<class T>
			typename std::vector<T>::const_iterator SynchronizedList<T>::begin() const
			{
				return m_syncedList.begin();
			}
			template<class T>
			typename std::vector<T>::const_iterator SynchronizedList<T>::end() const
			{
				return m_syncedList.end();
			}

		}
	}
}