#pragma once
/////////////////////////////////////////////////////////////////////
// Tester.h - Prototype for Test Harness child Tester              //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  This package defines one class, Tester:
*  - uses DllLoader to load libraries and extract tests.
*  - provides pointer to its member logger to each test, then
*    executes it, using the infrastructure from TestUtilities.
*
*  Required Files:
*  ---------------
*  Tester.h, Tester.cpp
*  DllLoader.h, DllLoader.cpp
*  ISingletonLogger.h, SingletonLogger.h
*  ITest.h, TestUtilities.h
*  Properties.h, Properties.cpp
*  FileUtilities.h, FileUtilities.cpp
*  FileSystem.h, FileSystem.cpp
*  StringUtilities.h
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 21 Oct 2018
*  - first release
*
*/
#include "../TestRequest/TestRequest.h"
#include "../Dll_Loader_With_Hosted_Resource/DllLoader/DllLoader.h"
#include "../Utilities/SingletonLogger/ISingletonLogger.h"
#include "../Utilities/SingletonLogger/SingletonLogger.h"
#include "../Utilities/Properties/Properties.h"
#include <vector>
#include <string>

#include "../CppCommWithFileXfer/Message/Message.h"

using namespace MsgPassingCommunication;
namespace Testing
{
  using namespace Utilities;

  ///////////////////////////////////////////////////////////////////
  // Tester class
  // - Loads TestRequests from requestPath.
  //   Each TestRequest provides names of Dlls to load and test
  // - Loads Test Dynamic Link Libraries (Dlls) from libPath.
  // - Executes tests defined in each Dll.
  // - Each tests logs ouputs using Tester class's logger

  using Requests = std::vector<TestRequest>;

  class Tester
  {
  public:
    Tester();
    ~Tester();

    Property<Requests> requests;
    Property<std::string> requestPath;
    Property<std::string> libPath;
    Property<ILog*> pLog;

    void loadTestRequests(TestRequest& testReq);
    void loadLibraries(TestRequest& tr);
    bool doTests();
    void setLibraryPath(std::string& path)
    {
      libPath(path);
    }
    void clear()
    {
      dllLoader_.clear();
    }
  private:
    DllLoader dllLoader_;
  };
}
