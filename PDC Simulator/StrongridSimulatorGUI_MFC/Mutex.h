#pragma once

#include <Windows.h>
#include <stdio.h>

namespace strongridsim
{	
	class Mutex
	{
	public:
		Mutex();
		~Mutex();

		bool Enter( int timeoutMs );
		void Exit();

	private:
		HANDLE m_hMutex; 
	};

	class MutexFragment
	{
	public:
		MutexFragment( Mutex* mutex, int timeout = INFINITE )
		{
			m_mutex = mutex;
			m_mutex->Enter(timeout);
		}

		~MutexFragment()
		{
			Finalize();
		}

		void Finalize()
		{
			if( m_mutex ) m_mutex->Exit();
			m_mutex = 0;
		}

	private:
		Mutex* m_mutex;
	};
}