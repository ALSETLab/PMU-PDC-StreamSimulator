#include "../StrongridBase/C37118Protocol.h"
#include "SimulatorDataTypes.h"

namespace strongridgui
{
	using namespace strongridbase;

	class C37118AdaptorUtils
	{
	public:
		static C37118PdcConfiguration Translate(const CPdcConfig& in_pdcConfig);

	protected:
		static C37118PmuConfiguration Translate(const CPmuConfig& in_pmuCfg);
		static C37118PhasorUnit Translate(const CPhasorUnit& in_phasorUnit );
		static C37118AnalogUnit Translate(const CAnalogUnit& in_analogUnit);
		static std::vector<C37118DigitalUnit> Translate(const std::vector<CDigitalUnitBit*>& in_digUnit );
	};
}