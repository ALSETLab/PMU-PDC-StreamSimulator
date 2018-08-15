#include <iostream>
#include <vector>
#include <string>

#define null 0

namespace stresstest
{
	class Exception
	{
	public:
		
		Exception(std::string strError) : m_error(strError) {}

		std::string ErrorMessage() { return m_error; }

	private:
		const std::string m_error;
	};

	struct PdcConfig
	{
		PdcConfig( std::string _ip, int _port, std::string _pdcid, int _version ) : 
			IP(_ip), Port(_port), PdcId(_pdcid), Version(_version)
		{
		}

		std::string IP;
		int Port;
		std::string PdcId;
		int Version;
	};

	struct ProgramConfig
	{
		std::vector<PdcConfig> PdcPool;
	};
}