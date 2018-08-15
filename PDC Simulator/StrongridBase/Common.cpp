#include <Windows.h>
#include "common.h"
#include <time.h>

using namespace strongridbase;

tm TimeConversionHelper::SecondsSinceEpochToDateTime(uint64_t SecondsSinceEpoch)
{
	tm blah;
	gmtime_s(&blah, (time_t*)&SecondsSinceEpoch);
	return blah;
}

uint32_t TimeConversionHelper::GetSocByDateTime(const tm* tms)
{
	return mktime((tm*)tms);
}