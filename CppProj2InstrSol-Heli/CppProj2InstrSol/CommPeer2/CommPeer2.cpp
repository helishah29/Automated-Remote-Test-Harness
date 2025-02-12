/////////////////////////////////////////////////////////////////////
// CommPeer2.cpp - Demonstrates message passing communication      //
// ver 1.0                                                         //
// Heli Shah, CSE687 - Object Oriented Design, Fall 2018           //
/////////////////////////////////////////////////////////////////////
/*
*
* CommPeer2 is the Mother for the project 3
*
*  Package Operations:
*  -------------------
*  This package demonstrates use of classes Comm, Message, and EndPoint.
*  It uses a thread to collect messages from Peer1 while sending messages
*  to Peer1.
*
*  Required Files:
*  ---------------
*    CommPeer1.cpp, CommPeer2.cpp
*
*  Required Projects:
*  ------------------
*  - Message          constructs and parses HTTP style messages
*  - MsgPassingComm   Sends and Retrieves messages
*  - Sockets          Used by Comm to send and receive strings and bytes
*  - Utilities        This will be replaced by an updated Utilities module
*  
*  Maintenance History:
*  --------------------
*  ver 1.1 : 13 Nov 2018
*  - changed according to the project requirements
*  ver 1.0 : 11 Oct 2018
*  - first release
*
*  Planned Changes:
*  ----------------
*  Convert to new Utilities Module.  That will affect:
*  - Message          will use properties
*  - MsgPassingComm   will use new singletonlogger
*  - Sockets          will use new singletonlogger
*/


#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include "../CppCommWithFileXfer/Message/Message.h"
#include "../Process/Process.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <functional>
#include "../CppCommWithFileXfer/Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include "../TestRequest/TestRequest.h"

using namespace MsgPassingCommunication;

using namespace Testing;
void sndMsgs(Comm& comm, EndPoint epTo, EndPoint epFrom, BlockingQueue<Message> &readyQ, BlockingQueue<Message> &testRequestQ)
{
  Message msg;
  //EndPoint eptoo("localhost", 8083);
  //msg.to(eptoo);
  msg.from(epFrom);
  int i = 3;
  while (readyQ.size() && testRequestQ.size()) {
	  Message readyMsg = readyQ.deQ();
	  Message testi = testRequestQ.deQ();
	  TestRequest tr;
	  testi.to(readyMsg.from());

	  std::cout << "\n Sending message to child to test dll\n";
	  std::cout << readyMsg.from().toString()<<"\n";

	  comm.postMessage(testi);	 
  }
}

void getMsgs(Comm& comm, BlockingQueue<Message>& readyQ, BlockingQueue<Message>& testRequestQ)
{

  Message msg;
  //EndPoint eptoo("localhost", 8083);
  while (true)
  {
	//std::cout << "message recieved\n";
    msg = comm.getMessage();
	//std::cout << msg.attribute << "\n";
	if (msg.containsKey("childReady")) {
		readyQ.enQ(msg);
		std::cout << "\n Message recieved from child no:\n"<<msg.from().toString()<<"\n";
		sndMsgs(comm, msg.from(), msg.to(), readyQ, testRequestQ);
	}
	else if (msg.containsKey("TestRequest")) {
		std::cout << "\n Message recieved from client \n";
		testRequestQ.enQ(msg);
		sndMsgs(comm, msg.from(), msg.to(), readyQ, testRequestQ);

	}
	else if (msg.containsKey("DemoSomeReq")) {
		std::cout << "\n DemoSomeReq recieved from client and sending to Child process \n";
		testRequestQ.enQ(msg);
		sndMsgs(comm, msg.from(), msg.to(), readyQ, testRequestQ);

	}
	//sndMsgs(comm, msg.from(), msg.to(), readyQ, testRequestQ);

    if (msg.command() == "stop")
      break;
  }
}
int main()
{
	// make mother and add the 2 Q
  std::cout << "\n  Demonstrating Peer2 in Peer-To-Peer Comm Demo";
  std::cout << "\n ===============================================";

  ::SetConsoleTitle(L"Mother");

  EndPoint epPeer1("localhost", 8083);
  EndPoint epPeer2("localhost", 8082);
  Comm comm(epPeer2, "Mother");
  comm.start();

  std::string savePath = comm.setSaveFilePath("../SaveFile");
  std::cout << "\n  saving files to \"" << savePath << "\"";
  std::string sendPath = comm.setSendFilePath("../SendFile");
  std::cout << "\n  sending files from \"" << sendPath << "\"";

  BlockingQueue<Message> readyQ;
  BlockingQueue<Message> testRequestQ;
  // tproc, below, will compile to a functor
  // - operator() contains the code between { and }
  // - comm will be a reference member
  
  Process p;
  p.title("Child 1");
  std::string appPath = "../x64/Debug/TestHarnessCore.exe";
  p.application(appPath);

  std::string cmdLine = "/A 8083";
  p.commandLine(cmdLine);

  std::cout << "\n  starting process: \"" << appPath << "\"";
  std::cout << "\n  with this cmdlne: \"" << cmdLine << "\"";
  p.create();
  
  Process c;
  c.title("Child 2");
  std::string appPath2 = "../x64/Debug/TestHarnessCore.exe";
  c.application(appPath2);

  cmdLine = "/A 8084";
  c.commandLine(cmdLine);

  std::cout << "\n  starting process: \"" << appPath2 << "\"";
  std::cout << "\n  with this cmdlne: \"" << cmdLine  << "\"";
  c.create();
  
/*
  Message fileMsg;
  fileMsg.to(epPeer2);
  fileMsg.from(epPeer1);
  fileMsg.value("sendingFile");   // required to send file
  fileMsg.name("Peer2");          // for display
  fileMsg.file("Comm.h");         // defines file to send from sendPath
  std::cout << "\n  " + comm.name() + " posting: " + fileMsg.name();
  comm.postMessage(fileMsg);
*/
  

  std::function<void()> tproc = [&]() { getMsgs(comm,readyQ,testRequestQ); };
  std::thread tget(tproc);
  tget.detach();

  std::function<void()> tproc1 = [&]() { sndMsgs(comm,epPeer1,epPeer2,readyQ,testRequestQ); };
  std::thread tsend(tproc1);
  tsend.detach();

  //sndMsgs(comm, epPeer1, epPeer2, readyQ, testRequestQ);

  //getMsgs(comm,readyQ,testRequestQ);

  std::cout << "\n  Press enter to quit: ";
  std::cin.get();
  //comm.stop();
  std::cout << "\n\n";
}