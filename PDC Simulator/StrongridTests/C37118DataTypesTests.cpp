#include <WinSock2.h>
#include "C37118DataTypesTests.h"
#include "Assert.h"
#include "../StrongridBase/C37118Protocol.h"

using namespace tests;
using namespace strongridbase;

void C37118PmuDataFramePhasorRealImagTest_CreateByRealImagInt()
{
	int16_t actualReal = 22, actualImag = 55;
	C37118PmuDataFramePhasorRealImag ph = C37118PmuDataFramePhasorRealImag::CreateByRealImag(actualReal, actualImag);

	// Method under test
	float real, imag;
	ph.getRealImagAsFloat(&real, &imag);

	// Assert - values should not change
	Assert::AreEqual(22.0f, real);
	Assert::AreEqual(55.0f, imag);
}

void C37118PmuDataFramePhasorRealImagTest_CreateByRealImagFloat()
{
	float actualReal = 22.6f, actualImag = 55.8f;
	C37118PmuDataFramePhasorRealImag ph = C37118PmuDataFramePhasorRealImag::CreateByRealImag(actualReal, actualImag);

	// Method under test
	float real, imag;
	ph.getRealImagAsFloat(&real, &imag);

	// Assert - values should not change
	Assert::AreEqual(22.6f, real);
	Assert::AreEqual(55.8f, imag);
}

void C37118PmuDataFramePhasorRealImagTest_CreateByPolarCoordsFloat()
{
	float mag = 562.0f, anglerad = 1.345f;
	C37118PmuDataFramePhasorRealImag phFromPolar = C37118PmuDataFramePhasorRealImag::CreateByPolarMag(mag, anglerad);

	float real, imag;
	phFromPolar.getRealImagAsFloat(&real,&imag);
	C37118PmuDataFramePhasorRealImag phToRect = C37118PmuDataFramePhasorRealImag::CreateByRealImag(real, imag);
	float resultMag, resultAngle;
	phToRect.getMagAngleAsFloat(&resultMag, &resultAngle);

	// Assert - should be ... something
	Assert::AreEqual(mag, resultMag);
	Assert::AreEqual(anglerad, resultAngle);
}


void C37118PmuDataFramePhasorRealImagTest_CreateByPolarCoordsInt16()
{
	// Create polar coords by integer
	uint16_t mag = 562;
	int16_t anglerad = (int16_t)(1.34 * pow(10,4));
	C37118PmuDataFramePhasorRealImag phFromPolar = C37118PmuDataFramePhasorRealImag::CreateByPolarMag(mag, anglerad);

	// Create
	float magFloat, angleFloat;
	phFromPolar.getMagAngleAsFloat(&magFloat, &angleFloat);

	Assert::AreEqual(mag, magFloat, 0.01f);
	Assert::AreEqual(1.34f, angleFloat, 0.01f);
}


void C37118PmuDataFramePhasorRealImagTest_GetPolarCoordsAsInt16()
{
	// Create polar coords by integer
	C37118PmuDataFramePhasorRealImag phFromPolar = C37118PmuDataFramePhasorRealImag::CreateByPolarMag(562.0f, 1.34f);
	
	// Create
	uint16_t mag;
	int16_t angle;
	phFromPolar.getMagAngleAsInt16(&mag, &angle);

	// test..
	Assert::AreEqual(562.0f, mag, 0.01f);
	Assert::AreEqual(1.34f * (float)pow(10,4),angle, 0.01f);
}

void C37118FracSecCreateAndParse_ReliableClock_CreateAndParseLoopbackTest()
{
	// Create a "reliable" clock
	C37118FracSec fracSec_reliable = C37118FracSec::Create(53, 1, true, 2 );

	int outLeapSecOffset;
	bool outLeapSecPending, outIsRealiable;
	float outTimeClockMaxErrorSec;
	fracSec_reliable.GetParsedQuality(&outLeapSecOffset, &outLeapSecPending, &outTimeClockMaxErrorSec, &outIsRealiable);

	// Test parsed result
	Assert::AreEqual(53, fracSec_reliable.FractionOfSecond);
	Assert::IsTrue(outLeapSecPending);
	Assert::IsTrue(outIsRealiable);
	Assert::AreEqual( pow(10.0f,-8.0f), outTimeClockMaxErrorSec, 0.01f);
}

void C37118FracSecCreateAndParse_UnreliableClock_CreateAndParseLoopbackTest()
{
	// Create a "reliable" clock
	C37118FracSec fracSec = C37118FracSec::Create(27, -1, false, 0xF );

	int outLeapSecOffset;
	bool outLeapSecPending, outIsRealiable;
	float outTimeClockMaxErrorSec;
	fracSec.GetParsedQuality(&outLeapSecOffset, &outLeapSecPending, &outTimeClockMaxErrorSec, &outIsRealiable);

	// Test parsed result
	Assert::AreEqual(27, fracSec.FractionOfSecond);
	Assert::IsFalse(outLeapSecPending);
	Assert::IsFalse(outIsRealiable);
}

void C37118PmuDataFrameStat_BitSetTestBits_BitsAreSet()
{
	C37118PmuDataFrameStat stat(0x0);

	// Test setting data error code
	stat = C37118PmuDataFrameStat(0x0); stat.setDataErrorCode(0x3);
	Assert::AreEqual(0x3, stat.getDataError());

	// Test setting pmu sync flag
	stat = C37118PmuDataFrameStat(0x0); stat.setPmuSyncFlag(true);
	Assert::AreEqualBool(true, stat.getPmuSyncFlag());

	// Test setting sorting flag
	stat = C37118PmuDataFrameStat(0x0); stat.setDataSortingFlag(true);
	Assert::AreEqualBool(true, stat.getDataSortingFlag());

	// Test pmu trigger flag
	stat = C37118PmuDataFrameStat(0x0); stat.setPmuTriggerFlag(true);
	Assert::AreEqualBool(true, stat.getPmuTriggerFlag() );

	// Test set config change flag
	stat = C37118PmuDataFrameStat(0x0); stat.setConfigChangeFlag(true);
	Assert::AreEqualBool(true, stat.getConfigChangeFlag() );

	// Test data modifier flag
	stat = C37118PmuDataFrameStat(0x0); stat.setDataModifiedFlag(true);
	Assert::AreEqualBool(true, stat.getDataModifiedFlag() );

	// Test time quality code
	stat = C37118PmuDataFrameStat(0x0); stat.setTimeQualityCode(0x3);
	Assert::AreEqual(0x3, stat.getTimeQualityCode() );

	// Test unlock time code
	stat = C37118PmuDataFrameStat(0x0); stat.setUnlockTimeCode(0x3);
	Assert::AreEqual(0x3, stat.getUnlockTimeCode() );

	// Test trigger reason code
	stat = C37118PmuDataFrameStat(0x0); stat.setTriggerReasonCode(0x3);
	Assert::AreEqual(0x3, stat.getTriggerReasonCode() );
}


void C37118PmuDataFrameStat_BitUnsetTestBits_BitsAreUnset()
{
	C37118PmuDataFrameStat stat(0xFFFF);

	// Test removing data error code
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setDataErrorCode(0x0);
	Assert::AreEqual(0x0, stat.getDataError());

	// Test removing pmu sync flag
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setPmuSyncFlag(false);
	Assert::AreEqualBool(false, stat.getPmuSyncFlag());

	// Test removing sorting flag
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setDataSortingFlag(false);
	Assert::AreEqualBool(false, stat.getDataSortingFlag());

	// Test removing pmu trigger flag
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setPmuTriggerFlag(false);
	Assert::AreEqualBool(false, stat.getPmuTriggerFlag() );

	// Test removing config change flag
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setConfigChangeFlag(false);
	Assert::AreEqualBool(false, stat.getConfigChangeFlag() );

	// Test removing data modifier flag
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setDataModifiedFlag(false);
	Assert::AreEqualBool(false, stat.getDataModifiedFlag() );

	// Test removing time quality code
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setTimeQualityCode(0x0);
	Assert::AreEqual(0x0, stat.getTimeQualityCode() );

	// Test removing unlock time code
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setUnlockTimeCode(0x0);
	Assert::AreEqual(0x0, stat.getUnlockTimeCode() );

	// Test removing trigger reason code
	stat = C37118PmuDataFrameStat(0xFFFF); stat.setTriggerReasonCode(0x0);
	Assert::AreEqual(0x0, stat.getTriggerReasonCode() );
}

C37118DataTypesTests::C37118DataTypesTests()
{
	PushTestMethod("C37118PmuDataFramePhasorRealImagTest_CreateByRealImagInt", &C37118PmuDataFramePhasorRealImagTest_CreateByRealImagInt);
	PushTestMethod("C37118PmuDataFramePhasorRealImagTest_CreateByRealImagFloat", &C37118PmuDataFramePhasorRealImagTest_CreateByRealImagFloat);
	PushTestMethod("C37118PmuDataFramePhasorRealImagTest_CreateByPolarCoordsFloat", &C37118PmuDataFramePhasorRealImagTest_CreateByPolarCoordsFloat);
	PushTestMethod("C37118PmuDataFramePhasorRealImagTest_CreateByPolarCoordsInt16", &C37118PmuDataFramePhasorRealImagTest_CreateByPolarCoordsInt16);
	PushTestMethod("C37118PmuDataFramePhasorRealImagTest_GetPolarCoordsAsInt16", &C37118PmuDataFramePhasorRealImagTest_GetPolarCoordsAsInt16);
	PushTestMethod("C37118FracSecCreateAndParse_ReliableClock_CreateAndParseLoopbackTest", &C37118FracSecCreateAndParse_ReliableClock_CreateAndParseLoopbackTest);
	PushTestMethod("C37118FracSecCreateAndParse_UnreliableClock_CreateAndParseLoopbackTest", &C37118FracSecCreateAndParse_UnreliableClock_CreateAndParseLoopbackTest);
	PushTestMethod("C37118PmuDataFrameStat_BitSetTestBits", &C37118PmuDataFrameStat_BitSetTestBits_BitsAreSet );
	PushTestMethod("C37118PmuDataFrameStat_BitUnsetTestBits_BitsAreUnset", &C37118PmuDataFrameStat_BitUnsetTestBits_BitsAreUnset );
}


std::string C37118DataTypesTests::TestClassName()
{
	return "C37118DataTypesTests";
}

