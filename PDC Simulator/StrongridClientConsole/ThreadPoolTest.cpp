#include <winsock2.h>
#include <Windows.h>
#include <iostream>
#include <iomanip>
#include <process.h>
#include <sstream>
#include <string>
#include <iomanip>

#include "main.h"
#include "../StrongridClientBase/PdcClient.h"
#include "../StrongridBase/common.h"

using namespace strongridclientbase;
using namespace std;

namespace ThreadPoolTest {

	struct ThreadPoolJobMediator
	{
		std::vector<WSAPOLLFD> sockArr;
		std::vector<PdcClient*> clients;
	};


	volatile bool isActive = true;

	const int BASE_PORT = 12400;
	const int NUM_PDC_INSTANCES = 1;
	const int NUM_WORKERS = 4;

	std::vector<int> CheckPortsForDataToRead(WSAPOLLFD* socketArray, int sockArrayLength, int  timeoutMs)
	{		
		int ret = WSAPoll(socketArray, sockArrayLength, timeoutMs);	

		std::vector<int> output(sockArrayLength, -1);
		if( ret > 0 )
		{
			for( int i = 0; i < sockArrayLength; ++i )
			{
				if( socketArray[i].revents & (POLLRDNORM | POLLERR | POLLHUP)  ) output[i] = socketArray[i].fd;
			}
		}
		return output;
	}

	void PmuWorkerProc(void* obj)
	{
		ThreadPoolJobMediator* med = (ThreadPoolJobMediator*)obj;

		// Create an array for the socket vector
		WSAPOLLFD* sockArr = new WSAPOLLFD[med->sockArr.size()];
		for( int i = 0; i < med->sockArr.size(); ++i )
			sockArr[i] = med->sockArr[i];

		// Read dataframes ..
		int framesReceivedTotal = 0;
		while( isActive )
		{
			std::vector<int> dataToRead = CheckPortsForDataToRead(sockArr, med->sockArr.size(), 5000);
			for( int i = 0; i < med->sockArr.size(); ++i )
			{
				if( dataToRead[i] <= 0 )
					continue;

				med->clients[i]->ReadDataFrame(30000);
				C37118PdcDataFrame frame = med->clients[i]->GetPdcDataFrame();
				//std::cout << (int)frame.HeaderCommon.Sync.LeadIn;
				if( ++framesReceivedTotal % (med->sockArr.size() * 50) == 0 )
					cout << ".";
			}
		}

		_endthread();
	}

	void ThreadPoolMain()
	{
		Sleep(5000);
		std::cout << "Connecting to #" << NUM_PDC_INSTANCES << " pdc's.." << std::endl;

		std::vector<PdcClient*> clients;
		std::vector<WSAPOLLFD> listenSockArray;

		// Initialize and connect
		for( int i = 0; i < NUM_PDC_INSTANCES; ++i )
		{
			int port = BASE_PORT + (i%7);
			PdcClient* pdcClient = new PdcClient("::1", port, i);
			pdcClient->Connect();
			clients.push_back(pdcClient);

			WSAPOLLFD polldata;
			polldata.fd = pdcClient->GetSocketDescriptor();
			polldata.events = POLLIN;
			listenSockArray.push_back(polldata);			
		}
	
		// Read configuration sequentially, "because hey why not"
		for( std::vector<PdcClient*>::iterator iter = clients.begin(); iter != clients.end(); ++iter )
		{
			std::cout << "Reading configuration..";
			(*iter)->ReadConfiguration(30000);
			std::cout << "done." << std::endl;
		}

		// Activate RTD
		for( std::vector<PdcClient*>::iterator iter = clients.begin(); iter != clients.end(); ++iter )
		{
			std::cout << "Activating RTD" << std::endl;
			(*iter)->StartDataStream();
		}
		std::cout << "Entering realtime-data mode.." << std::endl << std::endl;

		// Fire up 'n' workers and delegate the job
		ThreadPoolJobMediator* threadObjs = new ThreadPoolJobMediator[NUM_WORKERS];
		for( int i = 0; i < clients.size(); ++i )
		{
			threadObjs[i%NUM_WORKERS].clients.push_back(clients[i]);
			threadObjs[i%NUM_WORKERS].sockArr.push_back(listenSockArray[i]);
		}

		// start the workers
		for( int i = 0; i < NUM_WORKERS; ++i )
		{
			_beginthread(PmuWorkerProc, 0, &threadObjs[i]);
		}

		// Wait here until user stops the program
		cin.get();
		isActive = false;
		Sleep(1000);
	}
}