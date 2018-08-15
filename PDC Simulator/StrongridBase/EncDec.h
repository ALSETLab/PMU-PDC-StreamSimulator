#pragma once
#include <stdint.h>
#include "common.h"

namespace strongridbase
{
	class EncDec
	{
	public:
		static int32_t get_S32( char* data, int* offset );
		static int16_t get_S16( char* data, int* offset );
		static int8_t get_S8( char* data, int* offset );
		static uint32_t get_U32( char* data, int* offset );
		static uint16_t get_U16( char* data, int* offset );
		static uint8_t get_U8( char* data, int* offset );
		static float get_Single( char* data, int* offset );
		static double get_Double( char* data, int* offset );
		static std::string get_String( char* data, int strLen, int* offset );

		static void put_S32( char* data, int32_t val, int* offset );
		static void put_S16( char* data, int16_t val, int* offset );
		static void put_S8( char* data, int8_t val, int* offset );
		static void put_U32( char* data, uint32_t val, int* offset );
		static void put_U16( char* data, uint16_t val, int* offset );
		static void put_U8( char* data, uint8_t val, int* offset );
		static void put_Single( char* data, float val, int* offset );
		static void put_Double( char* data, double val, int* offset );
		static void put_String( char* data, std::string val, int maxLength, int* offset );

		// Convert between network byte order and host network order
		static int32_t ToNetByteOrder( int32_t val );
		static int32_t ToHostByteOrder( int32_t val );
		static int16_t ToNetByteOrder( int16_t val );
		static int16_t ToHostByteOrder( int16_t val );
		static uint32_t ToNetByteOrder( uint32_t val );
		static uint32_t ToHostByteOrder( uint32_t val );
		static uint16_t ToNetByteOrder( uint16_t val );
		static uint16_t ToHostByteOrder( uint16_t val );
		static float ToNetByteOrder( float val );
		static float ToHostByteOrder( float val );
	};
}