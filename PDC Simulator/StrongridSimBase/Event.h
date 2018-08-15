#pragma once

#include <vector>

namespace strongridsim
{
	class PDC;
	typedef void (*PdcEventHandler)(void* ACT, void* eventdata);	

	class Event
	{
		struct EvtMd
		{
			EvtMd(PdcEventHandler evthnd, void* act)
			{
				Handle = evthnd;
				ACT = act;
			}

			PdcEventHandler Handle;
			void* ACT;
		};

	public:
		Event()
		{
		}

		void AddHandler( PdcEventHandler evthandler, void* act )
		{
			m_eventList.push_back(EvtMd(evthandler, act));
		}

		void Publish(void* objdata)
		{
			for( std::vector<EvtMd>::iterator iter = m_eventList.begin(); iter != m_eventList.end(); ++iter )
				(*(iter->Handle))(iter->ACT,objdata);
		}

	private:
		std::vector<EvtMd> m_eventList; 
	};
}