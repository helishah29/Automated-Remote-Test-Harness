/////////////////////////////////////////////////////////////////////
// CommPeer1.cpp - Demonstrates message passing communication      //
// ver 1.0                                                         //
// Heli Shah, CSE687 - Object Oriented Design, Fall 2018           //
/////////////////////////////////////////////////////////////////////
/*
*
* CommPeer1 is the Client for project 3
* 
* Microsoft Visual Studio Community 2017
* Version 15.8.5
* 2017 Microsoft Corporation
*
*  Package Operations:
*  -------------------
*  This package demonstrates use of classes Comm, Message, and EndPoint.
*  It uses a thread to collect messages from Peer2 while sending messages
*  to Peer2.
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
*
*/

#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include "../CppCommWithFileXfer/Message/Message.h"
#include <iostream>
#include <thread>
#include <functional>
#include <windows.h>
#include "../TestRequest/TestRequest.h"


using namespace MsgPassingCommunication;
using namespace Testing;

void sndMsgs(Comm& comm, EndPoint epTo, EndPoint epFrom)
{
  Message msg;
  msg.to(epTo);
  msg.from(epFrom);

  std::cout << "\n Sending DemoSomeReq from client to mother";
  std::this_thread::sleep_for(std::chrono::seconds(5));
  TestRequest tr;
  tr.author("Heli");
  tr.name("testharness");
  //tr.addDll("TestDriver1.dll");
  tr.addDll("TestDriver2.dll");
  msg.attribute("TestRequest", tr.toString());
  std::cout << "\n Message sent is:" << tr.toString();
  comm.postMessage(msg);

  std::cout << "\n Sending message from client to mother";
  std::this_thread::sleep_for(std::chrono::seconds(5));
  TestRequest tr1;
  tr1.author("Heli");
  tr1.name("testharness");
  tr1.addDll("TestDriver1.dll");
  msg.attribute("TestRequest", tr1.toString());
  std::cout << "\n Message sent is:" << tr1.toString();
  comm.postMessage(msg);
}

void getMsgs(Comm& comm)
{
  Message msg;
  while (true)
  {
    msg = comm.getMessage();
	if (msg.containsKey("result")) {
		std::cout << "Test run at:" << msg.from().toString() << " has "<< msg.value("result");
		std::cout << "\n test passed";
	}
    std::cout << "\n Client recvd  message \"" << msg.name() << "\"";
    if (msg.command() == "stop")
      break;
  }
}

int main()
{
  std::cout << "\n  Demonstrating Peer1 in Peer-To-Peer Comm Demo";
  std::cout << "\n ===============================================";

  ::SetConsoleTitle(L"Client");

  EndPoint epPeer1("localhost", 8081);
  EndPoint epPeer2("localhost", 8082);
  Comm comm(epPeer1, "Peer1");
  comm.start();

  std::string savePath = comm.setSaveFilePath("../SaveFile");
  std::cout << "\n  saving files to \"" << savePath << "\"";
  std::string sendPath = comm.setSendFilePath("../SendFile");
  std::cout << "\n  sending files from \"" << sendPath << "\"";

  Message fileMsg;
  fileMsg.to(epPeer2);
  fileMsg.from(epPeer1);
  fileMsg.value("sendingFile");   // required to send file
  fileMsg.name("Peer1");          // for display
  fileMsg.file("Comm.h");         // defines file to send from sendPath
  std::cout << "\n  " + comm.name() + " posting: " + fileMsg.name();
  comm.postMessage(fileMsg);

  std::function<void()> tproc = [&]() { getMsgs(comm); };
  std::thread tget(tproc);
  tget.detach();

  sndMsgs(comm, epPeer2, epPeer1);

  Message msg;
  msg.command("stop");    // stop receiver
  comm.postMessage(msg);
  msg.command("quit");    // stop sender
  comm.postMessage(msg);
  std::cout << "\n  Press enter to quit: ";
  std::cin.get();
  comm.stop();
  std::cout << "\n\n";
}
