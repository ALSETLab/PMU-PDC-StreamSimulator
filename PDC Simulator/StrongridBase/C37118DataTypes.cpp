#include <bitset>
#include "common.h"
#include "C37118Protocol.h"
#include "EncDec.h"


using namespace strongridbase;


C37118FracSec C37118FracSec::Create( uint32_t fracSec, int leapSecOffset, bool leapSecPending, int clockErrorLevel)
{
	C37118FracSec out;
	out.FractionOfSecond = fracSec;
	out.TimeQuality = 0;

	// Set direction bit (bit6 - 0 for add, 1 for delete)
	if( leapSecOffset < 0 ) out.TimeQuality |= (0x40);

	// Set leap second ocurred bit (bit5)
	if( leapSecOffset != 0 ) out.TimeQuality |= (0x20);

	// Set leap second pending bit (bit4)
	if( leapSecPending ) out.TimeQuality |= (0x10);

	// Set quality indicator
	out.TimeQuality |= (0x0F & clockErrorLevel);
		
	return out;
}

void C37118FracSec::GetParsedQuality( int* outLeapSecOffset, bool* outLeapSecPending, float* outTimeClockMaxErrorSec, bool* outIsRealiable ) const
{
	const int leapDirSec = ((0x40 & TimeQuality) == 0) ? 1 : 0;
	const bool leapOcurred = (0x20 & TimeQuality) != 0;
	*outLeapSecOffset = leapOcurred ? leapDirSec : 0;
	*outLeapSecPending = (0x10 & TimeQuality) != 0;
	*outIsRealiable = true;

	const uint8_t accuracyCode = 0x0F & TimeQuality;
	if( accuracyCode == 0 ) 
		*outTimeClockMaxErrorSec = 0.0f;
	else if( accuracyCode > 0 && accuracyCode < 0xF )
		*outTimeClockMaxErrorSec = powf(10, -(float)accuracyCode);
	else {
		*outIsRealiable = false;
		*outTimeClockMaxErrorSec = 0;
	}
}


C37118DataRate C37118DataRate::CreateByRawC37118Format( int16_t datarateRaw )
{
	C37118DataRate tmp;
	tmp.m_datarateRaw = datarateRaw;
	return tmp;
}

C37118DataRate C37118DataRate::CreateByFramesPerSecond( float fps )
{
	C37118DataRate tmp;
	if( fps >= 1 )
		tmp.m_datarateRaw = (uint16_t)fps;
	else if( fps > 0 )
		tmp.m_datarateRaw = (uint16_t)(-(1.0f / fps));
	else // -negative/0 => 0 fps (invalid)
		tmp.m_datarateRaw = 0;

	return tmp;
}

int16_t C37118DataRate::RawDataRate() const
{
	return m_datarateRaw;
}

float C37118DataRate::FramesPerSecond() const
{
	if( m_datarateRaw >= 0 )
		return m_datarateRaw;
	else
		return 1.0f / (float)m_datarateRaw;
}


// ---------------------------------------------------------------------------------------------
//  C37118ContIdx
// ---------------------------------------------------------------------------------------------


C37118ContIdx::C37118ContIdx(uint16_t rawval)
{
	m_raw = rawval;
}

C37118ContIdx C37118ContIdx::CreateByC37118Raw( uint16_t rawValue )
{
	return C37118ContIdx (rawValue);
}

C37118ContIdx C37118ContIdx::CreateAsFrameInSequence( int frameIdx, int numFrames )
{
	if( numFrames == 1 ) // first and only..
		return C37118ContIdx(0);
	else if( frameIdx + 1 < numFrames ) // One of many, but NOT the last one
		return C37118ContIdx(frameIdx+1);
	else 
		return C37118ContIdx(0xFFFF); // Last frame
}


int C37118ContIdx::GetCurrentFrameIndex() const
{
	if( m_raw == 0 ) return 0; // First and only frame
	else if( m_raw > 0 && m_raw < 0xFFFF )
		return m_raw - 1; // Convert to 0-based index
	else
		return 0xFFFF; // signal an error
}

bool C37118ContIdx::IsLastFrame() const
{
	if( m_raw == 0) return true; // First and last frame
	else if( m_raw == 0xFFFF ) return true; // last frame
	else return false; // not the last frame..
}

uint16_t C37118ContIdx::GetRawC37118Value() const
{
	return m_raw;
}




 // Getters..
uint8_t C37118PmuDataFrameStat::getDataError() const // BIT 15-14
{
	return (m_raw & 0xC000) >> 14;
}

bool C37118PmuDataFrameStat::getPmuSyncFlag() const // Bit 13
{	
	return std::bitset<16>(m_raw).test(13);
}

bool C37118PmuDataFrameStat::getDataSortingFlag() const // Bit 12
{
	return std::bitset<16>(m_raw).test(12);
}

bool C37118PmuDataFrameStat::getPmuTriggerFlag() const // Bit 11
{
	return std::bitset<16>(m_raw).test(11);
}

bool C37118PmuDataFrameStat::getConfigChangeFlag() const // Bit 10
{
	return std::bitset<16>(m_raw).test(10);
}

bool C37118PmuDataFrameStat::getDataModifiedFlag() const // Bit 9 
{
	return std::bitset<16>(m_raw).test(9);
}

uint8_t C37118PmuDataFrameStat::getTimeQualityCode() const // Bit 6-8
{
	return (m_raw & 0x01C0) >> 6;	
}

uint8_t C37118PmuDataFrameStat::getUnlockTimeCode() const // Bit 4-5
{
	return (m_raw & 0x0030) >> 4;	
}

uint8_t C37118PmuDataFrameStat::getTriggerReasonCode() const
{
	return (m_raw & 0x000F);	
}


// Setters
void C37118PmuDataFrameStat::setDataErrorCode(uint8_t errCode) // Bit 14-15
{	
	m_raw = (m_raw & ~(0x3 << 14)) | ((errCode & 0x3) << 14);
}

void C37118PmuDataFrameStat::setPmuSyncFlag(bool isset) // Bit 13
{
	m_raw = (m_raw & ~(0x1 << 13)) | ((isset & 0x1) << 13);
}

void C37118PmuDataFrameStat::setDataSortingFlag(bool isset) // Bit 12
{
	m_raw = (m_raw & ~(0x1 << 12)) | ((isset & 0x1) << 12);
}

void C37118PmuDataFrameStat::setPmuTriggerFlag(bool isset) // Bit 11
{
	m_raw = (m_raw & ~(0x1 << 11)) | ((isset & 0x1) << 11);
}

void C37118PmuDataFrameStat::setConfigChangeFlag(bool isset) // Bit 10
{
	m_raw = (m_raw & ~(0x1 << 10)) | ((isset & 0x1) << 10);
}

void C37118PmuDataFrameStat::setDataModifiedFlag(bool isset) // Bit 9
{
	m_raw = (m_raw & ~(0x1 << 9)) | ((isset & 0x1) << 9);
}

void C37118PmuDataFrameStat::setTimeQualityCode(uint8_t code) // Bit 6-8
{	
	m_raw = (m_raw & ~(0x7 << 6)) | ((code & 0x7) << 6);
}

void C37118PmuDataFrameStat::setUnlockTimeCode(uint8_t code) // Bit 4-5
{
	m_raw = (m_raw & ~(0x3 << 4)) | ((code & 0x3) << 4);
}

void C37118PmuDataFrameStat::setTriggerReasonCode(uint8_t code) // Bit 0-3
{
	m_raw = (m_raw & ~(0xF << 0)) | ((code & 0xF) << 0);
}

uint16_t C37118PmuDataFrameStat::ToRaw() const
{
	return m_raw;
}


// ------------------------------------------------------------------------------------------------------------------------
// C37118PmuDataFramePhasorRealImag
// ------------------------------------------------------------------------------------------------------------------------


void ConvertRealImagToMagAngle(float real, float imag, float* refMag, float* refAngle )//Conversion function for change from rect to polar
{
	*refMag = sqrtf(pow(real,2) + pow(imag,2) );
	*refAngle = atanf(imag/real);
}

void ConvertMagAngleToRealImag(float mag, float angle, float* refReal, float* refImag )//Conversion function for change from polar to rect
{
	*refReal = cos(angle) * mag;
	*refImag = sin(angle) * mag;
}

C37118PmuDataFramePhasorRealImag C37118PmuDataFramePhasorRealImag::CreateByRealImag(int16_t real, int16_t imag)//Set real and imaginary value in integer format
{
	C37118PmuDataFramePhasorRealImag ph;
	ph.Real = (float)real;
	ph.Imag	= (float)imag;
	return ph;
}

C37118PmuDataFramePhasorRealImag C37118PmuDataFramePhasorRealImag::CreateByRealImag(float real, float imag)//Set real and imaginary float values 
{
	C37118PmuDataFramePhasorRealImag ph;
	ph.Real = real;
	ph.Imag	= imag;
	return ph;
}

C37118PmuDataFramePhasorRealImag C37118PmuDataFramePhasorRealImag::CreateByPolarMag(uint16_t magnitude, int16_t angleRad)//Converts to rectangular  
{                                                                                                                        //from degree angle
	float adjustedAngle = (float)((double)angleRad / pow(10,4));
	C37118PmuDataFramePhasorRealImag p;
	ConvertMagAngleToRealImag(magnitude, adjustedAngle, &p.Real, &p.Imag);
	return p;
}

C37118PmuDataFramePhasorRealImag C37118PmuDataFramePhasorRealImag::CreateByPolarMag(float magnitude, float angleRad)//Converts to rectungular from radian angle
{
	C37118PmuDataFramePhasorRealImag p;
	ConvertMagAngleToRealImag(magnitude, angleRad, &p.Real, &p.Imag );
	return p;
}

void C37118PmuDataFramePhasorRealImag::getRealImagAsFloat(float* refReal, float* refImag) const //Conversion to float
{
	*refReal = this->Real;
	*refImag = this->Imag;
}

void C37118PmuDataFramePhasorRealImag::getRealImagAsInt16(int16_t* refReal, int16_t* refImag) const //Conversion to int
{
	*refReal = (int16_t)this->Real;
	*refImag = (int16_t)this->Imag;
}


void C37118PmuDataFramePhasorRealImag::getMagAngleAsFloat(float* mag, float* angle) const//Calls Conversion from rect to polar
{
	ConvertRealImagToMagAngle(this->Real, this->Imag, mag, angle );
}

void C37118PmuDataFramePhasorRealImag::getMagAngleAsInt16(uint16_t* mag, int16_t* angle) const
{
	float a, b;
	ConvertRealImagToMagAngle(this->Real, this->Imag, &a, &b );
	*mag = (int16_t)a;
	*angle = (int16_t)(b * pow(10,4));
}




// ------------------------------------------------------------------------------------------------------------------------
// C37118PmuDataFrameAnalog
// ------------------------------------------------------------------------------------------------------------------------


C37118PmuDataFrameAnalog C37118PmuDataFrameAnalog::CreateByInt16(int16_t value)//Sets analog value 
{
	C37118PmuDataFrameAnalog ang;
	ang.Value = value*100000;
	return ang;
}

C37118PmuDataFrameAnalog C37118PmuDataFrameAnalog::CreateByFloat(float value)
{
	C37118PmuDataFrameAnalog ang;
	ang.Value = value*100000;
	return ang;
}
		

float C37118PmuDataFrameAnalog::getValueAsFloat() const
{
	return this->Value;
}

int16_t C37118PmuDataFrameAnalog::getValueAsInt16() const
{
	return (int16_t)this->Value;
}



// ------------------------------------------------------------------------------------------------------------------------
// C37118PmuDataFrameDigital
// ------------------------------------------------------------------------------------------------------------------------


C37118PmuDataFrameDigitalHelper C37118PmuDataFrameDigitalHelper::CreateByBoolArray(std::vector<bool> digBits)
{
	C37118PmuDataFrameDigitalHelper helper;
	for( std::vector<bool>::const_iterator iter = digBits.begin(); iter != digBits.end(); ++iter )
		helper.m_digValues.push_back(*iter);
	return helper;
}

C37118PmuDataFrameDigitalHelper C37118PmuDataFrameDigitalHelper::CreateByUint16Arr(std::vector<uint16_t> digWordArray)
{
	C37118PmuDataFrameDigitalHelper helper;
	for( std::vector<uint16_t>::const_iterator iter = digWordArray.begin(); iter != digWordArray.end(); ++iter )
		helper.PushDigWord(*iter);
	return helper;
}

void C37118PmuDataFrameDigitalHelper::PushDigWord(uint16_t word)
{	
	for( int i = 0; i < 16; ++i )
		m_digValues.push_back( (word & (0x1 << i)) >> i );
}

void C37118PmuDataFrameDigitalHelper::PushDigValue( bool bit )
{
	m_digValues.push_back(bit);
}


std::vector<bool> C37118PmuDataFrameDigitalHelper::ToBoolArray() const
{
	return m_digValues;
}

std::vector<uint16_t> C37118PmuDataFrameDigitalHelper::ToDigWord() const
{
	std::vector<uint16_t> output;

	int counter = 0;
	while( counter < m_digValues.size() )
	{
		uint16_t digword = 0;

		// Travese in blocks of 16 bits - set bits which correlate to a "true" value in the dig value array
		for( int i = 0; i < 16 && counter < m_digValues.size(); ++i, ++counter )
		{
			digword |= ((m_digValues[counter] & 0x1) << i);
		}

		// Add word to output
		output.push_back(digword);
	}

	return output;
}
