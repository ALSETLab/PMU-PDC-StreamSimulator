#include <Windows.h>
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

#include "Assert.h"
#include "Common.h"
#include "TestClassBase.h"
#include "EncDecTests.h"
#include "C37118ProtocolTests.h"
#include "C37118DataTypesTests.h"
#include "UtilsTests.h"

using namespace tests;

struct TestGroup
{
	std::string GroupName;
	std::vector<TestClassBase*> Tests;
};

int main()
{
	// All tests
	std::vector<TestGroup> testLists;	

	// Initialize testgroup - FUNDEMENTALS
	TestGroup testGroup_fundamentals;
	testGroup_fundamentals.GroupName = "Fundementals";
	testGroup_fundamentals.Tests.push_back( new EncDecTests() );
	testGroup_fundamentals.Tests.push_back( new C37118ProtocolTests() );
	testGroup_fundamentals.Tests.push_back( new C37118DataTypesTests() );
	testGroup_fundamentals.Tests.push_back( new UtilsTests() );

	// Add all groups
	testLists.push_back(testGroup_fundamentals);	
	
	// Run the tests - 
	for( std::vector<TestGroup>::iterator iterTestGroup = testLists.begin(); iterTestGroup != testLists.end(); ++iterTestGroup )
	{
		std::cout << "Executing testgroup: " << iterTestGroup->GroupName << "\n---------------------------------" << std::endl;
		
		// Iterate over each test in testgroup
		for( std::vector<TestClassBase*>::iterator testIter = iterTestGroup->Tests.begin(); testIter != iterTestGroup->Tests.end(); ++testIter )
		{
			std::cout << "Executing test: " << (*testIter)->TestClassName() << std::endl;
			std::vector<TestResult> result = (*testIter)->ExecuteTests();
			std::cout << std::endl;
		}

		std::cout << std::endl << std::endl;
	}

	std::cout << "\n\nAll tests completed. Press any key to exit";

	// Wait ..
	std::cin.get();	
}