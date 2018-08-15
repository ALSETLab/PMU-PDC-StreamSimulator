#include <WinSock2.h>
#include "EncDecTests.h"
#include "Assert.h"
#include "../StrongridBase/EncDec.h"

using namespace tests;
using namespace strongridbase;


void get_S32Test_ValueDecodedFromRawHex()
{
	char bytesToDecode[] = { 0x0, 0x0, 0x01, 0xD2 };  // hexVal = 0x1D2; // 466
	
	// Method under test
	int offset = 0;
	int value = ntohl( EncDec::get_S32(bytesToDecode, &offset ) );
	
	Assert::AreEqual(466, value );
	Assert::AreEqual(4, offset );
}

void put_S32Test_ValueEncodedToRawHex()
{
	int valueToEncode = 3156515;
	char data[4]; memset(data, 0, 4);
	char expected[] = { 0x0, 0x30, 0x2A, 0x23 };

	// Method under test
	int offset = 0;
	EncDec::put_S32(data, htonl(valueToEncode), &offset);
	
	Assert::AreEqual(data, 4, expected, 4 );
	Assert::AreEqual(4, offset );
}

void put_StringTest_StringIsWrittenToByteArray()
{
	std::string str = "this is a test";
	char arr[15];
	memset( arr, 0xAA, 15 );
	int offset = 0;
	char expected[] = { 't','h','i','s',' ', 'i','s',' ','a',' ','t','e','s','t',0xAA };

	EncDec::put_String(arr, str, str.length(), &offset );

	// Make sure the string was copied, and that 0xAA was not overwritten
	Assert::AreEqual(14, offset);
	Assert::AreEqual(expected,14+1,arr,14+1);
}


void put_StringTest_TooLongStringIsWrittenToByteArray_PartialStringWritten()
{
	std::string str = "this is a test";
	char arr[10];
	memset( arr, 0xAA, 10 );
	int offset = 0;
	char expected[] = { 't','h','i','s',' ', 'i','s',' ','a',0xAA };

	EncDec::put_String(arr, str, 9, &offset );

	// Make sure the string was copied, and that 0xAA was not overwritten
	Assert::AreEqual(9, offset);
	Assert::AreEqual(expected,10,arr,10);
}


void get_StringTest_StringIsReadFromByteArray()
{
	char data[] = { 't','h','i','s',' ', 'i','s',' ','a',' ','t','e','s','t',0xAA };

	int offset = 0;
	std::string retval = EncDec::get_String(data, 14, &offset );

	Assert::AreEqual(14, offset );
	Assert::AreEqual("this is a test", retval );
}


EncDecTests::EncDecTests()
{
	PushTestMethod("get_S32Test_ValueDecodedFromRawHex", &get_S32Test_ValueDecodedFromRawHex);
	PushTestMethod("put_S32Test_ValueEncodedToRawHex", &put_S32Test_ValueEncodedToRawHex);
	PushTestMethod("put_StringTest_StringIsWrittenToByteArray", &put_StringTest_StringIsWrittenToByteArray);
	PushTestMethod("get_StringTest_StringIsReadFromByteArray", &get_StringTest_StringIsReadFromByteArray);
	PushTestMethod("put_StringTest_TooLongStringIsWrittenToByteArray_PartialStringWritten", &put_StringTest_TooLongStringIsWrittenToByteArray_PartialStringWritten);
}

std::string EncDecTests::TestClassName()
{
	return "EncDecTests";
}

