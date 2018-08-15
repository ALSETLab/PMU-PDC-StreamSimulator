#define TIXML_USE_STL

#include <vector>
#include "../StrongridDLL/Strongrid.h"
#include "Dependencies\tinyxml2.h"//Change to tinyxml2.h???
#include <fstream>
#include "Common.h"

using namespace std;

#define null 0

namespace stresstest
{
	ProgramConfig ReadProgramConfig(std::string configFile)
	{	
		ProgramConfig output;

		tinyxml2::XMLDocument doc( "config.xml" );
		if(doc.LoadFile( "config.xml") != 0 )	throw Exception("config.xml did not parse correctly or could not be found.");

		// Get a reference to the pdc pool
		tinyxml2::XMLElement* tixRoot = doc.FirstChildElement();
		tinyxml2::XMLElement* tixPdcPool = tixRoot->FirstChildElement("PdcPool");
		if( tixPdcPool == null ) throw Exception("Missing <PdcPool> element");

		// Traverse the entire pdc pool and create list of all PDCs
		tinyxml2::XMLElement* tixPdcCursor = tixPdcPool->FirstChildElement();
		while( tixPdcCursor != null)
		{
			// extract the attributes
			const char* str_ip = tixPdcCursor->Attribute("IP") ;
			const char* str_port = tixPdcCursor->Attribute("Port");
			const char* str_pdcid = tixPdcCursor->Attribute("PDCID");
			const char* str_version = tixPdcCursor->Attribute("Version");
			if( str_ip == null || str_port == null ) throw new Exception("Missing either 'IP', 'Port' or 'PDCID', attribute under <Pdc ../> element.");

			int tmp_version = str_version == 0 ? 1 : atoi(str_version);

			// Add to output conifig object
			output.PdcPool.push_back( PdcConfig(str_ip, atoi(str_port), str_pdcid, tmp_version) );

			// Advance cursor
			tixPdcCursor = tixPdcCursor->NextSiblingElement();
		}

		return output;
	}
}