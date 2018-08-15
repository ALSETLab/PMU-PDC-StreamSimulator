#include "stdafx.h"
#include "../StrongridSimBase/IPmuDataFeed.h"
#include <map>
#include "ChannelDataFeed.h"

using namespace strongridsim;


namespace strongridgui
{	
	enum InputOverrideType
	{
		CONSTANT,
		FILEBASED
	};

	class BridgedPmuDataFeed : public IPmuDataFeed
	{
	public:
		BridgedPmuDataFeed(IPmuDataFeed* pmuDataFeed);
		~BridgedPmuDataFeed();

		// Set new PMU datafeed - the old one is deleted 
		void SetPmuDataFeed( IPmuDataFeed* ownDatafeed );		

		// Set "override" values "per signal"
		void SetPhasorOverride( int phasorIndex, IPhasorDataFeed* phDataFeed );
		void SetAnalogOverride( int analogIndex, IAnalogDataFeed* angDataFeed );
		void SetDigitalOverride( int digitalIdx, IDigitalDataFeed* digDataFeed );
		void SetFrequency( float freq, float dfreq );

		IPhasorDataFeed* GetPhasorOverride( int phasorIndex );
		IAnalogDataFeed* GetAnalogOverride( int analogIndex );
		IDigitalDataFeed* GetDigitalOverride( int digitalIndex );

	public: // IPmuDataFeed members
		virtual void GetPhasorValue( int phIdx, float* part1, float* part2 );
		virtual float GetAnalogValue( int angIdx );
		virtual bool GetDigitalBit( int digIdx );
		virtual void GetFrequency( float* frequency, float* deltafrequency );
		virtual C37118PmuDataFrameStat GetFrameStat(); // Connected directly to m_masterFrameStat
		virtual C37118PmuDataFrameStat* GetFrameStatRef();
		
		virtual void SignalNextFrame();

	private:
		C37118PmuDataFrameStat m_masterFrameStat; // Overrides the one in IPmuDataFeed
		float m_masterFrequency, m_masterDeltaFrequency;

		IPmuDataFeed* m_pmuDataFeed; // TODO: refactor away - all channels should be 'overridden' by default and therefore never be null
		std::map<int, IPhasorDataFeed*> m_phasorDataFeedMap;
		std::map<int, IAnalogDataFeed*> m_analogDataFeedMap;
		std::map<int, IDigitalDataFeed*> m_digitalDataFeedMap;
	};
}