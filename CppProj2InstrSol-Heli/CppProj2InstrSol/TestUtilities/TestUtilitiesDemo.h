#pragma once
#pragma warning(disable : 4996)
///////////////////////////////////////////////////////////////////////
// TestUtilitiesDemo.h - facilities for single-user test harness     //
// ver 2.1                                                           //
// Language:    C++, Visual Studio 2017                              //
// Application: Most Projects, CSE687 - Object Oriented Design       //
// Author:      Gokul Baskaran, gobaskar@syr.edu, OOD Fall 2018		 //
// Source:      Jim Fawcett, Syracuse University, CST 4-187          //
//              jfawcett@twcny.rr.com                                //
///////////////////////////////////////////////////////////////////////
/*
* Package Operations:
* -------------------
* This package provides three classes and a struct:
* - TestExecutor    Executes single test in the context of a try-catch block
* - TestExecutive   Uses TestExecutor to run a sequence of tests
* - TestHelpers     Provides methods to display file names and contents
* - Cosmetic        Displays newline when instance goes out of scope
*
* Required Files:
* ---------------
*   TestUtilities.h
*
* Maintenance History:
* --------------------
* ver 2.1 : 04 Sep 2018
* - added TestExecutive::clearTests()
* - minor changes to display layout
* ver 2.0 : 28 Aug 2018
* - major change from tests defined by lambdas
*   to tests defined by classes derived from ITest interface
* ver 1.0 : 12 Jan 2018
* - first release
* - taken from Utilities/TestUtilities package
*
* Notes:
* ------
* - Designed to provide all functionality in header file.
* - Implementation file only needed for test and demo.
*
* Planned Additions and Changes:
* ------------------------------
* - none yet
*/

#include <vector>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <memory>
#include "../DemoSomeReqs/FileSystem.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
//#include "../StringUtilities/StringUtilities.h"
//#include "../TestUtilities/FileSystem.h"
//#include "../StringUtilities/StringUtilities.h"
#include "LoggerHelp.h"
#include <ctime>

namespace Utilities
{
  using Path = std::string;
  using Message = std::string;
  using Line = size_t;
  using File = std::string;
  using Files = std::vector<File>;

  //LoggerHelp logFile;
  //std::string fileSpec = "..\\..\\LogFile.txt";
  //std::ofstream outfileStrm(filespec, std::ios::out);

  /////////////////////////////////////////////////////////////////////
  // TestExecutor class
  // - supports execution of callable objects for testing in the
  //   context of a try-catch block.

  struct ITest {
    virtual bool test() = 0;
    virtual ~ITest() {}
  };

  class TestExecutor
  {
  public:
    bool execute(std::shared_ptr<ITest> pTest, const std::string& name, const std::string& desc, std::ostream&);
  private:
    void check(bool result, std::ostream& out);
  };
  //----< execute tests in the context of a try-catch block >----------

  bool TestExecutor::execute(std::shared_ptr<ITest> pTest, const std::string& name, const std::string& desc, std::ostream& out)
  {
	
    bool result = false;
    try
    {
      result = pTest->test();  // execute test which may throw an exception
      check(result, out);

	  time_t x = time(0);
	  char* date = ctime(&x);

      out << " -- \"" << name << "\"  ";
	  out << desc;
	  //out << "\nTest was executed at - "<<
    }
    catch (std::exception& ex)
    {
      check(false, out);
      out << " -- \"" << name << "\"\n";
      out << ex.what();
    }

    return result;
  }
  //----< display test results >---------------------------------------

  void TestExecutor::check(bool result, std::ostream& out)
  {
    if (result)
      out << "\n  passed";
    else
      out << "\n  failed";
  }

  ///////////////////////////////////////////////////////////////////////
  // TestExecutive class
  // - executes a sequence of tests with the help of TestExecutor

  class TestExecutive
  {
  public:
    using TestItem = struct {
      std::shared_ptr<ITest> pTest;
      std::string testName;
	  std::string desc;
    };
    using Tests = std::vector<TestItem>;

    bool doTests();
    void registerTest(std::shared_ptr<ITest> pTest, const std::string& testName);
    void registerTest(const TestItem& ts);
    void clearTests();
	void loggerFile();
  private:
    Tests tests_;  // collection of test items
	LoggerHelp logs;   // object to store logs 
  };
  //----< store test in test collection for later execution >--------

  inline void TestExecutive::registerTest(std::shared_ptr<ITest> pTest, const std::string& testName)
  {
    TestItem ts{ pTest, testName };
    tests_.push_back(ts);
    // push_back copies the test item
    // - that makes a new shared_ptr sharing the same test as the original
    // - it also makes a copy of the test name string
  }

  inline void TestExecutive::registerTest(const TestItem& ts)
  {
    tests_.push_back(ts);
    // push_back copies the test item
    // - that makes a new shared_ptr sharing the same test as the original
    // - it also makes a copy of the test name string
  }

  inline void TestExecutive::clearTests()
  {
    tests_.clear();
  }

  //----< execute each stored test >---------------------------------

  inline bool TestExecutive::doTests()
  {
    TestExecutor tester;
	std::stringstream stringStream;
    bool result = true;
    for (auto item : tests_)
    {
      bool tResult = tester.execute(item.pTest, item.testName, item.desc, stringStream);

	  if (tResult == false)
		  result = false;
    }
	
	logs.write(stringStream.str());  // calling logs function to document for each time a test is called
	time_t x = time(0);
	char* date = ctime(&x);
 
	logs.write("\n\n These Tests was Run at - ");
	logs.write(date);
	logs.write(" Author - Gokul Baskaran \n \n");

    return result;
  }

  void TestExecutive::loggerFile()
  {
	  logs.addStream(&std::cout);
	  std::string filename = "..//..//LoggerFile.txt";
	  
	  logs.addStream(new std::ofstream(filename, std::ios::out));
  }
  // Function to make logs in a txt file

  ///////////////////////////////////////////////////////////////////
  // TestHelpers class
  // - provides support for testing - will move to TestUtilities

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

}

