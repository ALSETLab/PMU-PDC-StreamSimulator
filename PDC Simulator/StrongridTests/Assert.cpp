#include "Assert.h"
#include "Common.h"

using namespace tests;

void Assert::AreEqual( int32_t val1, int32_t val2 )
{
	if( val1 != val2 ) throw TestException("Values are not equal");
}

void Assert::AreEqualU32( uint32_t val1, uint32_t val2 )
{
	if( val1 != val2 ) throw TestException("Values are not equal");
}

void Assert::AreEqual( char* arr1, int arr1len, char* arr2, int arr2len )
{
	if( arr1len != arr2len ) throw TestException("Arrays differ in length");

	for( int i = 0; i < arr1len; ++i )
		if( arr1[i] != arr2[i] ) 
			throw TestException("Values are not equal");
}


void Assert::AreEqual( std::string val1, std::string val2 )
{
	if( val1.compare(val2) != 0 ) throw TestException("String-values are not equal.");
}

void Assert::AreEqualBool( bool a, bool b )
{
	if( a != b ) throw TestException("Values are not equal");
}

		
void Assert::IsFalse( bool b )
{
	if( b != false ) throw TestException("Value is not false");
}

void Assert::IsTrue( bool b )
{
	if( b != true ) throw TestException("Value is not true");
}


void Assert::AreEqual( float val1, float val2, float tol )
{
	if( abs(val1 - val2) > tol) throw TestException("Floating point values are not equal/within tolerance");
}