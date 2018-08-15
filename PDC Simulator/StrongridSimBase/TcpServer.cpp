
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <mstcpip.h>
#include <Windows.h>
#include <process.h>
#include <sstream>
#include <vector>
#include "../StrongridBase/common.h"
#include "TcpServer.h"
#include <functional>
#include <algorithm>

#define BACKLOG 10     // how many pending connections queue will hold

using namespace strongridsim;
using namespace strongridbase;

TcpServer::TcpServer( int listenPort, OnConnection oc, void* onConnectionObjData)
{
	InitializeWindowsSocket();
	m_listenPort = listenPort;
	m_onConnection = oc;
	m_onCallbackObjData = onConnectionObjData;
}

TcpServer::~TcpServer()
{
	Shutdown();
}


void TcpServer::Start()
{
	if( m_isActive == true ) throw Exception("TcpServer already started.");
	m_isActive = true;

	// Start listening..
	m_threadIsAlive = true;
	_beginthread(&TcpServerProc, 0, this);
}

void TcpServer::Shutdown()
{
	if( m_isActive == false ) return;
	m_isActive = false;
	while( m_threadIsAlive ) Sleep(50);
}

std::vector<Socket*> TcpServer::SelectReadyToReadSockets(std::vector<Socket*> sockVec, int timeoutMs)
{	

	WSAPOLLFD* ufds = new WSAPOLLFD[sockVec.size()];
	for( int i = 0; i < sockVec.size(); ++i )
	{
		ufds[i].fd = sockVec[i]->getSocketFd();
		ufds[i].events = POLLIN;
	}

	int ret = WSAPoll(ufds, sockVec.size(), timeoutMs);	

	std::vector<Socket*> output;
	if( ret > 0 )
	{
		for( int i = 0; i < sockVec.size(); ++i )
		{
			if( ufds[i].revents & (POLLRDNORM | POLLERR | POLLHUP)  ) output.push_back(sockVec[i]);
		}
	}

	delete [] ufds;

	return output;
}



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}



void TcpServer::HandlenewConnectionEvent(TcpServer* tcpSrv, int listener)
{
    struct sockaddr_storage remoteaddr; // client address
    char remoteIP[INET6_ADDRSTRLEN];

	// handle new connections
    int addrlen = sizeof remoteaddr;
    int newfd = accept(listener,
        (struct sockaddr *)&remoteaddr,
        &addrlen);


	// Disable congestion control
	BOOL bOptVal = TRUE;
	int iResult = setsockopt(newfd,  IPPROTO_TCP, TCP_NODELAY, (char *) &bOptVal, sizeof(BOOL));
    if (iResult == SOCKET_ERROR) 
        throw Exception("Unable to disable TCP congestion control on socket.");

    if (newfd == -1) {
        perror("accept");
    } else {
        printf("selectserver: new connection from %s on "
            "socket %d\n",
            inet_ntop(remoteaddr.ss_family,
                get_in_addr((struct sockaddr*)&remoteaddr),
                remoteIP, INET6_ADDRSTRLEN),
            newfd);

		std::string remoteAddrStr = std::string(inet_ntop(remoteaddr.ss_family,
                get_in_addr((struct sockaddr*)&remoteaddr),
                remoteIP, INET6_ADDRSTRLEN));
		
		Socket* socket = new Socket(newfd, remoteAddrStr);
		(*tcpSrv->m_onConnection)(socket, tcpSrv->m_onCallbackObjData);
    }
}

void TcpServer::TcpServerProc(void* mythis)
{
	TcpServer* tcpSrv = (TcpServer*)mythis;

	fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
	fd_set except_fds;
    std::vector<int> listeners;     // listening socket descriptor
    char yes=1;        // for setsockopt() SO_REUSEADDR, below
	int rv = 0;

    struct addrinfo hints, *ai, *p;

    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);

    // Get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
	std::stringstream ss; ss << tcpSrv->m_listenPort;
    if ((rv =  getaddrinfo(NULL, ss.str().c_str(), &hints, &ai)) != 0) {
        fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
        exit(1);
    }

	int fdmax = 0;
    
	// Setup listener
    for(p = ai; p != NULL; p = p->ai_next) {
        int listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
        if (listener < 0) { 
            continue;
        }

        // lose the pesky "address already in use" error message
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            closesocket(listener);
            continue;
        }

		    // Make sure the port was bound
		if (p == NULL)
			throw Exception("TcpServer error - failed to bind to port");

		// listen
		if (listen(listener, 10) == -1) {
			throw Exception("TcpServer error - failed to listen on port");
	    }
		
		// Add to listener array
		listeners.push_back(listener);
		
		// add the listener to the master set
	    FD_SET(listener, &master);

		fdmax = max(listener,fdmax);
    }
    freeaddrinfo(ai); // all done with this

	timeval seltimeout;
	seltimeout.tv_sec = 1;
	seltimeout.tv_usec = 0;

    // main loop
    while( tcpSrv->m_isActive )
	{
		// Wait for read/connection event
        read_fds = master; 
		except_fds = master;
        if (select(fdmax+1, &read_fds, NULL, NULL, &seltimeout) == -1) 
			throw Exception("TcpServer error - select call failed");
			

        // run through the existing connections looking for data to read
        for(int i = 0; tcpSrv->m_isActive && i <= fdmax; i++) 
		{
            if (FD_ISSET(i, &read_fds)) 
			{
				std::vector<int>::iterator iterListener = std::find(listeners.begin(), listeners.end(), i);
				if ( iterListener != listeners.end() ) 
					HandlenewConnectionEvent(tcpSrv, *iterListener);
				else
					printf("other");
            } 

			/*if( FD_ISSET(i, &except_fds) )
			{
				printf("blahalh");
			}*/
        } 
    }

	for( int i = 0; i < listeners.size(); ++i )
		closesocket(listeners[i]);
    
	tcpSrv->m_threadIsAlive = false;
	// Cleanup thread
	_endthread();
}


bool TcpServer::m_win32Initialized = false;

void TcpServer::InitializeWindowsSocket()
{
	// Only initialize once
	if( m_win32Initialized == true ) return;
	m_win32Initialized = true;

	// Initialize window32 socket
	 WSADATA wsaData;

    if (WSAStartup(MAKEWORD(2,0), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed.\n");
		throw Exception("Unable to initialize winsock2!");
    }
}