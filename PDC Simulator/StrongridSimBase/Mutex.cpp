#include "Mutex.h"

using namespace strongridsim;

Mutex::Mutex()
{
	m_hMutex = CreateMutex( 
        NULL,              // default security attributes
        FALSE,             // initially not owned
        NULL);             // unnamed mutex
}

Mutex::~Mutex()
{
	CloseHandle(m_hMutex);
}

bool Mutex::Enter( int timeoutMs )
{
	int result = WaitForSingleObject( m_hMutex, timeoutMs );
	return result == WAIT_OBJECT_0;
}

void Mutex::Exit()
{
	ReleaseMutex(m_hMutex);
}