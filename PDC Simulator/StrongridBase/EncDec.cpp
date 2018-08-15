#include <WinSock2.h>
#include "EncDec.h"

using namespace strongridbase;

int32_t EncDec::get_S32( char* data, int* offset )
{	
	int32_t tmp = *((int32_t*)(data + *offset));
	*offset += sizeof(int32_t);
	return tmp;
}

int16_t EncDec::get_S16( char* data, int* offset )
{
	int16_t tmp = *((int16_t*)(data + *offset));
	*offset += sizeof(int16_t);
	return tmp;
}

int8_t EncDec::get_S8( char* data, int* offset )
{
	int8_t tmp = *((int8_t*)(data + *offset));
	*offset += sizeof(int8_t);
	return tmp;
}

uint32_t EncDec::get_U32( char* data, int* offset )
{
 	uint32_t tmp = *((uint32_t*)(data + *offset));
	*offset += sizeof(uint32_t);
	return tmp;
}

uint16_t EncDec::get_U16( char* data, int* offset )
{
	uint16_t tmp = *((uint16_t*)(data + *offset));
	*offset += sizeof(uint16_t);
	return tmp;
}

uint8_t EncDec::get_U8( char* data, int* offset )
{
	uint8_t tmp = *((uint8_t*)(data + *offset));
	*offset += sizeof(uint8_t);
	return tmp;
}

float EncDec::get_Single( char* data, int* offset )
{
	float tmp = *((float*)(data + *offset));
	*offset += sizeof(float);
	return tmp;
}

double EncDec::get_Double( char* data, int* offset )
{
	double tmp = *((double*)(data + *offset));
	*offset += sizeof(double);
	return tmp;
}

std::string EncDec::get_String( char* data, int strLen, int* offset )
{
	char* tmp = new char[strLen+1];
	memcpy(tmp,data+*offset,strLen);
	tmp[strLen] = 0; // force zero terminator
	*offset += strLen;
	std::string output(tmp);
	delete [] tmp;
	return output;
}

void EncDec::put_S32( char* data, int32_t val, int* offset )
{
	*(int32_t*)(data+*offset) = val;
	*offset += sizeof(int32_t);
}

void EncDec::put_S16( char* data, int16_t val, int* offset )
{
	*(int16_t*)(data+*offset) = val;
	*offset += sizeof(int16_t);
}

void EncDec::put_S8( char* data, int8_t val, int* offset )
{
	*(int8_t*)(data+*offset) = val;
	*offset += sizeof(int8_t);
}

void EncDec::put_U32( char* data, uint32_t val, int* offset )
{
	*(uint32_t*)(data+*offset) = val;
	*offset += sizeof(uint32_t);
}

void EncDec::put_U16( char* data, uint16_t val, int* offset )
{
	*(uint16_t*)(data+*offset) = val;
	*offset += sizeof(uint16_t);
}

void EncDec::put_U8( char* data, uint8_t val, int* offset )
{
	*(uint8_t*)(data+*offset) = val;
	*offset += sizeof(uint8_t);
}

void EncDec::put_Single( char* data, float val, int* offset )
{
	*(float*)(data+*offset) = val;
	*offset += sizeof(float);
}

void EncDec::put_Double( char* data, double val, int* offset )
{
	*(double*)(data+*offset) = val;
	*offset += sizeof(double);
}

void EncDec::put_String( char* data, std::string val, int maxLength, int* offset )
{
	memset(data+*offset, 0, maxLength);
	int adjLen = __min(maxLength,val.length());
	memcpy(data+*offset,val.c_str(),adjLen);
	*offset += maxLength;
}


int32_t EncDec::ToNetByteOrder( int32_t val )
{
	return htonl(val);
}

int32_t EncDec::ToHostByteOrder( int32_t val )
{
	return ntohl(val);
}

int16_t EncDec::ToNetByteOrder( int16_t val )
{
	return htons(val);
}

int16_t EncDec::ToHostByteOrder( int16_t val )\
{
	return ntohs(val);
}

uint32_t EncDec::ToNetByteOrder( uint32_t val )
{
	return htonl(val);
}

uint32_t EncDec::ToHostByteOrder( uint32_t val )
{
	return ntohl(val);
}
uint16_t EncDec::ToNetByteOrder( uint16_t val )
{
	return htons(val);
}
uint16_t EncDec::ToHostByteOrder( uint16_t val )
{
	return ntohs(val);
}

float EncDec::ToNetByteOrder( float val )
{
	uint32_t a = *((uint32_t*)&val); // reinterpret as uint32
	uint32_t b = htonl(a);			 // switch endianness: host-to-net
	float c = *((float*)&b);
	return c;
}


float EncDec::ToHostByteOrder( float val )
{
	uint32_t a = *((uint32_t*)&val); // reinterpret as uint32
	uint32_t b = ntohl(a);			 // switch endianness: net-to-host
	float c = *((float*)&b);
	return c;
}