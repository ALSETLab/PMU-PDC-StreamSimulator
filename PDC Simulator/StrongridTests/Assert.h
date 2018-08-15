#include <stdint.h>
#include <string>

namespace tests
{
	class Assert
	{
	public:
		static void AreEqual( int32_t val1, int32_t val2 );
		static void AreEqualU32( uint32_t val1, uint32_t val2 );
		static void AreEqual( char* arr1, int arr1len, char* arr2, int arr2len );
		static void AreEqual( std::string val1, std::string val2 );
		static void AreEqualBool( bool a, bool b );
		static void AreEqual( float val1, float val2, float tol );
				
		static void IsFalse( bool b );
		static void IsTrue( bool b );
	};
}