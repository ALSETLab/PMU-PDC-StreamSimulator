#pragma once
#include <time.h>
#include <string>
#include <stdint.h>

namespace strongridbase
{
	class Exception
	{
	public:
		Exception(const std::string error)
		{
			m_error = error;
		}

		std::string ExceptionMessage()
		{
			return m_error;
		}

	private:
		std::string m_error;
	};

	
	class SocketException : public Exception
	{
	public:
		SocketException(const std::string error) : Exception(error)
		{
		}
	};

	class SocketTimeout : public SocketException
	{
	public:
		SocketTimeout(const std::string error) : SocketException(error)
		{
		}
	};

	class TimeConversionHelper
	{
	public:
		static tm SecondsSinceEpochToDateTime(uint64_t SecondsSinceEpoch);
		static uint32_t GetSocByDateTime(const tm* tm);
	};
}