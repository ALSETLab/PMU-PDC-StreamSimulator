#include <string>

namespace tests
{
	class TestException
	{
	public:
		TestException(std::string message)
		{
			m_errMsg = message;
		}

		std::string ErrorMessage()
		{
			return m_errMsg;
		}

	private:
		std::string m_errMsg;
	};
}