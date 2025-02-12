/////////////////////////////////////////////////////////////////////
// Client.cpp - creates TestRequest and stores in requestPath      //
// ver 1.0                                                         //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  Package Operations:
*  -------------------
*  Client creates test request and stores in requestPath.
*  - This simulates what a Project #4 client does, e.g., creates
*    TestRequests and sends to TestHarness via a Comm channel.
*
*  Required Files:
*  ---------------
*  Client.cpp
*  TestRequest.h, TestRequest.cpp
*  DateTime.h, DateTime.cpp
*
*  Maintenance History:
*  --------------------
*  ver 1.0 : 23 Oct 2018
*  - first release
*/

#include "../TestRequest/TestRequest.h"
#include "../Utilities/FileUtilities/FileUtilities.h"
#include <iostream>

namespace Testing
{
  //----< mock test >------------------------------------------------
  /*
  * - This function is used build a TestRequest.
  * - It's used to demonstrate TestHarness Core operations.
  */
  void mock_Test()
  {
    TestRequest tr1;
    tr1.name("tr1");
    tr1.author("Fawcett");
    tr1.date(DateTime().now());
    tr1.request.valueRef().push_back("TestDriver1.dll");
    tr1.request.valueRef().push_back("TestDriver2.dll");

    Utilities::title("Created test request:");
    std::cout << "\n  " << tr1.toString();

    PathStr TrPath = "../TestRequests";
    PathStr fileSpec = TrPath + "/tr1.txt";
    std::wcout << ("\n\n  Writing to \"" + fileSpec + "\"").c_str();

    std::ofstream out;
    if (Utilities::openWriteFile(fileSpec, out))
    {
      out << tr1.toString();
    }
    std::cout << "\n";
  }
}

using namespace Testing;

int main()
{
  Utilities::Title("Demonstrating Client");

  mock_Test();
  
  std::cout << "\n";
  return 0;
}

