#include <iostream>
#include "TestClassBase.h"
#include "Common.h"

using namespace tests;

std::vector<TestResult> TestClassBase::ExecuteTests()
{
	std::vector<TestResult> testresults;

	for( std::vector<TestClassBase::TestMethodMediator>::iterator iter = m_testMethods.begin(); iter != m_testMethods.end(); ++iter )
	{
		TestResult tr;
		tr.testName =  iter->testName;

		try {
			// Execute testfunction
			TestMethod tm = (*iter).tm;
			(*tm)();

			tr.testOk = true;
		}
		catch( TestException te )
		{
			tr.error = te.ErrorMessage();
			tr.testOk = false;
		}
		catch( ... )
		{
			tr.error = "Unknown error";
			tr.testOk = false;
		}
		testresults.push_back( tr);

		// Log the result
		TestResult logTr = *testresults.rbegin();
		if( logTr.testOk ) std::cout << "Test '" << logTr.testName << "': OK" << std::endl;
		else std::cout << "Test '" << logTr.testName << "': " << logTr.error << std::endl;
	}

	return testresults;
}

void TestClassBase::PushTestMethod(std::string testName, TestClassBase::TestMethod tm)
{
	TestMethodMediator med;
	med.testName = testName;
	med.tm =  tm;
	m_testMethods.push_back(med);
}