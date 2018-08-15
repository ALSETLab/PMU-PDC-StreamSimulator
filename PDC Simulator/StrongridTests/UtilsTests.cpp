
#include <WinSock2.h>
#include "UtilsTests.h"
#include "Assert.h"
#include "../StrongridBase/common.h"

using namespace tests;
using namespace strongridbase;



void FromLocalTimeToSocToLocalTime_LoopbackTest()
{

	time_t t = time(0);

	tm retval = TimeConversionHelper::SecondsSinceEpochToDateTime(t);

	uint32_t total = TimeConversionHelper::GetSocByDateTime(&retval);

	Assert::AreEqual(1,1); // TODO fix this crap..
}

UtilsTests::UtilsTests()
{
	PushTestMethod("FromLocalTimeToSocToLocalTime_LoopbackTest", &FromLocalTimeToSocToLocalTime_LoopbackTest);
}

std::string UtilsTests::TestClassName()
{
	return "UtilsTests";
}

