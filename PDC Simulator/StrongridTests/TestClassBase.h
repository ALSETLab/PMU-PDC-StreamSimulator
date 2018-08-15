#pragma once
#include <string>
#include <vector>

namespace tests
{
	struct TestResult
	{
		std::string testName;
		bool testOk;
		std::string error;
	};
	
	class TestClassBase
	{
	public:
		typedef void(*TestMethod)(void);

		std::vector<TestResult> ExecuteTests();
		virtual std::string TestClassName() = 0;

	protected:
		struct TestMethodMediator
		{
			std::string testName;
			TestMethod tm;
		};

		void PushTestMethod(std::string testName, TestMethod tm);

	private:
		std::vector<TestMethodMediator> m_testMethods;
	};
}