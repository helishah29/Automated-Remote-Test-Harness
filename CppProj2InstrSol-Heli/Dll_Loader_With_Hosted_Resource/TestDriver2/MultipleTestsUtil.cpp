////////////////////////////////////////////////////////////////////////////
// MultipleTestsUtil.cpp : multiple test classes for CodeUtilities pkg    //
// ver 1.0                                                                //
//                                                                        //
// Application : OOD F18 Project 2 Help                                   //
// Platform    : VS17 Community - Windows 10 Professional x64             //
// Author      : heli shah, CSE687 - OOD, Fall 2018                     //
// Source      : Ammar Salman, EECS Department, Syracuse University       //
//               313/788-4694, hoplite.90@hotmail.com                     //
////////////////////////////////////////////////////////////////////////////
/*
*  Package description:
* ======================
*  This is a 'multiple tests' test driver DLL package. It follows the same
*  protocol defined in ITest.h package. For DllLoader, it doesn't care
*  if the TestDriver contains one or many Test Classes as it will attempt
*  to get the collection of ITest*.
*
*  Required files:
* =================
*  MultipleTestsUtil.cpp
*  ITests.h
*  CodeUtilities.h
*
*  Maintainence History:
* =======================
*  ver 2.0 : 07 Oct 2018
*  - replaced all tests
*  - Ammar's infrastructure has not been modified
*  ver 1.0 - first release
*/

#define IN_DLL
#define ARG_SIZE 256

#include "../ITests.h"
#include "../CodeUtilities/CodeUtilities.h"
#include "../../Utilities/FileSystem/FileSystem.h"
#include "../../Utilities/StringUtilities/StringUtilities.h"
#include "../../Utilities/DateTime/DateTime.h"

#include <iomanip>
#include <memory>
#include <iostream>
#include <string>
#include <string.h>

using namespace Utilities;
using namespace std;
using Path = std::string;
using Message = std::string;
using Line = size_t;
using File = std::string;
using Files = std::vector<File>;
class TestHelpers
{
public:
	static bool showDirContents(const Path& path, const Message& msg = "");
	static bool showFileLines(const Path& path, Line start, Line end, const Message& msg = "");
};

//----< show all C++ source files in path >------------------------

inline bool TestHelpers::showDirContents(const Path& path, const Message& msg)
{
	if (msg.size() > 0)
		Utilities::title(msg);
	std::cout << "\n  " << FileSystem::Path::getFullFileSpec(path);
	if (!FileSystem::Directory::exists(path))
	{
		std::cout << "\n  path does not exist";
		return false;
	}

	Files files = FileSystem::Directory::getFiles(path);
	for (auto file : files)
	{
		std::string ext = FileSystem::Path::getExt(file);
		if (ext == "h" || ext == "cpp")
			std::cout << "\n    " << file;
	}
	std::cout << "\n";
	return true;
}
//----< show specified contiguous lines from a code file >---------

inline bool TestHelpers::showFileLines(const Path& path, Line start, Line end, const Message& msg)
{
	if (msg.size() > 0)
		Utilities::title(msg);
	std::cout << "\n  " << FileSystem::Path::getFullFileSpec(path);

	std::ifstream file(path);
	if (!file.good())
	{
		std::cout << "\n  can't open file";
		return false;
	}

	std::string line;
	size_t count = 0;
	while (std::getline(file, line))
	{
		if (++count < start) continue;
		if (count > end) break;
		std::cout << "\n  " << std::setw(4) << count << " " << line;
	}
	std::cout << "\n";
	return true;
}

//----< displays newline when instance goes out of scope >---------

struct Cosmetic { ~Cosmetic() { std::cout << "\n"; } };


//////////////////////////////////////////////////////////////////
// Test class Demo1

class Demo1 : public ITest {
public:
  DLL_DECL bool test()
  {
    if (pRes_ == nullptr)
    {
      std::cout << "\n  no logger available";
      return false;
    }
    pRes_->setTerminator("");
    pRes_->write("\n  Testing Tester Package");
    pRes_->write("\n ========================");

    pRes_->write("\n  Test:   " + name());
    pRes_->write("\n  Author: " + author());
    pRes_->write("\n  " + Utilities::DateTime().now());

    pRes_->write("\n");
    pRes_->write("\n  Testing Demo1 class");
    pRes_->write("\n ---------------------");
    pRes_->write("\n  Test:   " + name());
    pRes_->write("\n  Author: " + author());
    pRes_->write("\n--Test always passes");
    return true;
  }
  DLL_DECL std::string name()
  {
    return "Demo1";
  }
  DLL_DECL std::string author()
  {
    return "Fawcett";
  }
  DLL_DECL void acceptHostedResource(ILog* pRes)
  {
    pRes_ = pRes;
  }
private:
  ILog* pRes_ = nullptr;
};

//////////////////////////////////////////////////////////////////
// Test class Demo2

class Demo2 : public ITest {
public:
  DLL_DECL bool test()
  {
    if (pRes_ == nullptr)
    {
      std::cout << "\n  no logger available";
      return false;
    }

    pRes_->write("\n");
    pRes_->write("\n  Testing Demo2 class");
    pRes_->write("\n ---------------------");

    pRes_->write("\n  Test:   " + name());
    pRes_->write("\n  Author: " + author());
    pRes_->write("\n--test always fails");
    return false;
  }
  DLL_DECL std::string name()
  {
    return "Demo2";
  }
  DLL_DECL std::string author()
  {
    return "Fawcett";
  }
  DLL_DECL void acceptHostedResource(ILog* pRes)
  {
    pRes_ = pRes;
  }
private:
  ILog* pRes_ = nullptr;
};
//////////////////////////////////////////////////////////////////
// Test class Demo3

class Demo3 : public ITest {
public:
  DLL_DECL bool test()
  {
    if (pRes_ == nullptr)
    {
      std::cout << "\n  no logger available";
      return false;
    }

    pRes_->write("\n");
    pRes_->write("\n  Testing Demo3 class");
    pRes_->write("\n ---------------------");

    pRes_->write("\n  Test:   " + name());
    pRes_->write("\n  Author: " + author());
    pRes_->write("\n--Test always throws");
    std::exception ex("\n  exception was thrown");
    throw ex;
    return true; // won't get here
  }
  DLL_DECL std::string name()
  {
    return "Demo3";
  }
  DLL_DECL std::string author()
  {
    return "Fawcett";
  }
  DLL_DECL void acceptHostedResource(ILog* pRes)
  {
    pRes_ = pRes;
  }
private:
  ILog* pRes_ = nullptr;
};


using Path = std::string;
using Message = std::string;
using Line = size_t;
using File = std::string;
using Files = std::vector<File>;

/*
Test Requirement 1 is to use Visual Studio 2017 and its C++ Windows Console Projects, as provided in the ECS computer labs.
This requirement has been fulfilled and taken care of.
*/
class TestRequirement1 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #1 - To use Visual Studio 2017 and its C++ Windows Console Projects, as provided in the ECS computer labs.";
		Path path = path_ + "/CommPeer1/";
		TestHelpers::showDirContents(path, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement1() { std::cout << "\n  deleting TestRequirement1"; }
};

/*
Test Requirement 2 is to use Windows Presentation Foundation (WPF) for all user display. 
*/
class TestRequirement2 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #2 - To use the Windows Presentation Foundation (WPF) for all user display.";
		Path path = path_ + "/GUI/MainWindow.xaml.cs";
		TestHelpers::showFileLines(path, 188, 255, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo2";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement2() { std::cout << "\n  deleting TestRequirement2"; }
};

/*
Test Requirement 3 is to assemble the working parts from Projects #1, #2, and #3 into a Client-Server configuration. Unlike conventional Client-Server architectures, the Client need not wait for a reply for a TestRequest from the TestHarness before sending additional TestRequests. 
*/

class TestRequirement3 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #3 - To assemble the working parts from Projects #1, #2, and #3 into a Client-Server configuration. Unlike conventional Client-Server architectures, the Client need not wait for a reply for a TestRequest from the TestHarness before sending additional TestRequests.";
		Path path = path_ + "/CommPeer2/CommPeer2.cpp";
		TestHelpers::showFileLines(path, 104, 166, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement3() { std::cout << "\n  deleting TestRequirement3"; }
};

/*
Test Requirement 4 to provide Graphical User Interface (GUI) for the client that supports selecting test libraires to send for testing, and supports displaying the test results. 
*/

class TestRequirement4 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #4 - To provide a Graphical User Interface (GUI) for the client that supports selecting test libraires to send for testing, and supports displaying the test results. ";
		Path path = path_ + "/GUI/MainWindow.xaml.cs";
		TestHelpers::showFileLines(path, 33, 122, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement4() { std::cout << "\n  deleting TestRequirement4"; }
};

/*
Test Requirement 5 is to provide message designs appropriate for this application. All messages are instances of the same Message class, but have a specified set of attributes and body contents suited for the intended task. 
*/

class TestRequirement5 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #5 - To provide message designs appropriate for this application. All messages are instances of the same Message class, but have a specified set of attributes and body contents suited for the intended task. ";
		Path path = path_ + "/CommPeer1/CommPeer1.cpp";
		TestHelpers::showFileLines(path, 80, 163, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement5() { std::cout << "\n  deleting TestRequirement5"; }
};

/*
Test Requirement 6 is to provide support sending TestRequest messages from Client to TestHarness Server.
*/

class TestRequirement6 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #6 - To provide support sending TestRequest messages from Client to TestHarness Server. ";
		Path path = path_ + "/TestHarnessCore/Tester.cpp";
		TestHelpers::showFileLines(path, 115, 163, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement6() { std::cout << "\n  deleting TestRequirement6"; }
};

/*
Test Requirement 7 To support correctly executing each TestRequest, as a sequence of tests, with each TestRequest running in its own Process Pool child process. 
*/

class TestRequirement7 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #7 - support correctly executing each TestRequest, as a sequence of tests, with each TestRequest running in its own Process Pool child process. ";
		Path path = path_ + "/CommPeer2/CommPeer2.cpp";
		TestHelpers::showFileLines(path, 55, 165, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement7() { std::cout << "\n  deleting TestRequirement7"; }
};

/*
Test Requirement 8 To support sending test results, via results messages, from the child process executing the tests to the Client that made the request. 
*/

class TestRequirement8 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #7 - To support sending test results, via results messages, from the child process executing the tests to the Client that made the request.  ";
		Path path = path_ + "/CommPeer2/CommPeer2.cpp";
		TestHelpers::showFileLines(path, 104, 166, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement8() { std::cout << "\n  deleting TestRequirement8"; }
};

/*
Test Requirement 9 To demonstrate correct test operations for two or more concurrent clients. You need to ensure that testing takes enough time to ensure that more than one child process is running at the same time.
*/

class TestRequirement9 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #9 - To demonstrate correct test operations for two or more concurrent clients. You need to ensure that testing takes enough time to ensure that more than one child process is running at the same time.   ";
		Path path = path_ + "/CommPeer2/CommPeer2.cpp";
		TestHelpers::showFileLines(path, 55, 103, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement9() { std::cout << "\n  deleting TestRequirement9"; }
};

/*
Test Requirement 10 - To include an automated unit test suite that demonstrates your Clients and TestHarness satisfy all of the functional requirements, above. 
*/

class TestRequirement10 : public ITest
{
	DLL_DECL bool test()
	{
		Message msg = "Req #10 - To include an automated unit test suite that demonstrates your Clients and TestHarness satisfy all of the functional requirements, above.  ";
		Path path = path_ + "/TestDriver1/ITests.h";
		TestHelpers::showFileLines(path, 71, 107, msg);
		return true;
	}
	DLL_DECL std::string name()
	{
		return "Demo3";
	}
	DLL_DECL std::string author()
	{
		return "heli shah";
	}
	DLL_DECL void acceptHostedResource(ILog* pRes)
	{
		pRes_ = pRes;
	}
private:
	ILog* pRes_ = nullptr;

private:
	const std::string path_ = "../";
public:
	~TestRequirement10() { std::cout << "\n  deleting TestRequirement10"; }
};

//////////////////////////////////////////////////////////////////
// Container that's responsible for returning a collection of the
// previous 2 test classes. It will return a collection of:
// 1) ITest* that points to an instance of TestProcessCmdArgs
// 2) ITest* that points to an instance of TestConverter

class TestCollection : public ITests {
	DLL_DECL std::vector<ITest*> tests();
};

// this is where each test class is instantiated and added to collection
DLL_DECL std::vector<ITest*> TestCollection::tests()
{
	std::vector<ITest*> tests_vec;
	tests_vec.push_back(new Demo1); 
  tests_vec.push_back(new Demo2);
  tests_vec.push_back(new Demo3);
  tests_vec.push_back(new TestRequirement1);
  tests_vec.push_back(new TestRequirement2);
  tests_vec.push_back(new TestRequirement3);
  tests_vec.push_back(new TestRequirement4);
  tests_vec.push_back(new TestRequirement5);
  tests_vec.push_back(new TestRequirement6);
  tests_vec.push_back(new TestRequirement7);
  tests_vec.push_back(new TestRequirement8);
  tests_vec.push_back(new TestRequirement9);
  tests_vec.push_back(new TestRequirement10);

	return tests_vec;
}

//////////////////////////////////////////////////////////////////////////////
// this section is where each Test Driver DLL completely differs from other
// test drivers. Although the same name can be used in all TestDrivers, the 
// actual instance of TestCollection is different in the way it returns 
// different collection of ITest*.

DLL_DECL ITests* get_ITests()
{
	return new TestCollection;
}


