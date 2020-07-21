/////////////////////////////////////////////////////////////////////
// Comm.h - message-passing communication facility                 //
// ver 2.0                                                         //
// Jim Fawcett, CSE687-OnLine Object Oriented Design, Fall 2017    //
/////////////////////////////////////////////////////////////////////

#include "Comm.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Cpp11-BlockingQueue/Cpp11-BlockingQueue.h"
#include <iostream>
#include <fstream>
#include <functional>
#include <algorithm>
#include <conio.h>

using namespace MsgPassingCommunication;
using namespace Sockets;
using SUtils = Utilities::StringHelper;

const std::string saveFilePath = "../SaveFiles";
const std::string sendFilePath = "../SendFiles";
const size_t BlockSize = 1024;
Socket::byte rwBuffer[BlockSize];

//----< constructor sets port >--------------------------------------

Receiver::Receiver(EndPoint ep, const std::string& name) : listener(ep.port), rcvrName(name)
{
  StaticLogger<1>::write("\n  -- starting Receiver");
}
//----< returns reference to receive queue >-------------------------

BlockingQueue<Message>* Receiver::queue()
{
  return &rcvQ;
}
//----< starts listener thread running callable object >-------------

template<typename CallableObject>
void Receiver::start(CallableObject& co)
{
  listener.start(co);
}
//----< stops listener thread >--------------------------------------

void Receiver::stop()
{
  listener.stop();
}
//----< retrieves received message >---------------------------------

Message Receiver::getMessage()
{
  StaticLogger<1>::write("\n  -- " + rcvrName + " deQing message");
  return rcvQ.deQ();
}
//----< constructor initializes endpoint object >--------------------

Sender::Sender(const std::string& name) : sndrName(name)
{
  lastEP = EndPoint();  // used to detect change in destination
}
//----< destructor waits for send thread to terminate >--------------

Sender::~Sender()
{
  if (sendThread.joinable())
    sendThread.join();
}
//----< starts send thread deQ, inspect, and send loop >-------------

void Sender::start()
{
  std::function <void()> threadProc = [&]() {
    while (true)
    {
      Message msg = sndQ.deQ();

      if (msg.command() == "quit")
      {
        StaticLogger<1>::write("\n  -- send thread shutting down");
        return;
      }
      StaticLogger<1>::write("\n  -- " + sndrName + " send thread sending " + msg.name());
      std::string msgStr = msg.toString();

      if (msg.to().address != lastEP.address || msg.to().port != lastEP.port)
      {
        connecter.shutDown();
        //connecter.close();
        StaticLogger<1>::write("\n  -- attempting to connect to new endpoint: " + msg.to().toString());
        if (!connect(msg.to()))
        {
          StaticLogger<1>::write("\n can't connect");
          continue;
        }
        else
        {
          StaticLogger<1>::write("\n  connected to " + msg.to().toString());
        }
      }
      if (msg.containsKey("file"))
      {
        sendFile(msg);
      }
      else
      {
        bool sendRslt = connecter.send(msgStr.length(), (Socket::byte*)msgStr.c_str());
      }
    }
  };
  std::thread t(threadProc);
  sendThread = std::move(t);
}
//----< stops send thread by posting quit message >------------------

void Sender::stop()
{
  Message msg;
  msg.name("quit");
  msg.command("quit");
  postMessage(msg);
  connecter.shutDown();
}
//----< attempts to connect to endpoint ep >-------------------------

bool Sender::connect(EndPoint ep)
{
  lastEP = ep;
  return connecter.connect(ep.address, ep.port);
}
//----< posts message to send queue >--------------------------------

void Sender::postMessage(Message msg)
{
  sndQ.enQ(msg);
}
//----< sends binary file >------------------------------------------
/*
*  - not implemented yet
*/
bool Sender::sendFile(Message msg)
{
  if (!msg.containsKey("file"))
    return false;
/::Sleep(100);
  }
  Message fileMsg(serverEP, clientEP);
  fileMsg.name("fileSender");
  fileMsg.file("logger.cpp");
  comm.postMessage(fileMsg);
  ::Sleep(200);

  Message stop;
  stop.name("stop");
  stop.to(serverEP);
  stop.command("stop");
  comm.postMessage(stop);
}
//----< handler for 2nd concurrent client >--------------------------

void ThreadProcClnt2()
{
  Comm comm(EndPoint("localhost", 9892), "client2Comm");
  comm.start();
  EndPoint serverEP("localhost", 9890);
  EndPoint clientEP("localhost", 9892);
  size_t IMax = 3;
  for (size_t i = 0; i < IMax; ++i)
  {
    Message msg(serverEP, clientEP);
    msg.name("client #2 : msg #" + Utilities::Converter<size_t>::toString(i));
    std::cout << "\n  " + comm.name() + " posting:  " << msg.name();
    comm.postMessage(msg);
    Message rply = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received: " << rply.name();
  }
  Message fileMsg(serverEP, clientEP);
  fileMsg.name("fileSender");
  fileMsg.file(---------------------------

  ~ClientHandler() 
  { 
    StaticLogger<1>::write("\n  -- ClientHandler destroyed;"); 
  }
  //----< set BlockingQueue >----------------------------------------

  void setQueue(BlockingQueue<Message>* pQ)
  {
    pQ_ = pQ;
  }
  //----< frame message string by reading bytes from socket >--------

  std::string readMsg(Socket& socket)
  {
    std::string temp, msgString;
    while (socket.validState())
    {
      temp = socket.recvString('\n');  // read attribute
      msgString += temp;
      if (temp.length() < 2)           // if empty line we are done
        break;
    }
    return msgString;
  }
  //----< receive file blocks >--------------------------------------
  /*
  *  - expects msg to contain file and contentLength attributes
  *  - expects to be connected to appropriate destination
  *  - these requirements are established in Sender::start()
  */
  bool receiveFile(Message msg)
  {
    std::string fileName = msg.file();
    std::string fileSpec = saveFilePath + "/" + fileName;
    std::ofstream saveStream(fileSpec, std::ios::binary);
    if (!saveStream.good())
    {
      return false;
    }
    while (true)
    {
      size_t blockSize = msg.contentLength();
      if (blockSize == 0)
        break;
      Socket::byte terminator;
      pSocket->recv(1, &terminator);
      pSocket->recv(blockSize, rwBuffer);
      saveStream.write(rwBuffer, blockSize);
      std::string msgString = readMsg(*pSocket);
      if (msgString.length() == 0)
      {
        break;
      }
      msg = Message::fromString(msgString);
      if (msg.contentLength() == 0)
        break;
    }
    saveStream.flush();
    saveStream.close();
    pQ_->enQ(msg);
    return true;
  }
  //----< reads messages from socket and enQs in rcvQ >--------------

  void operator()(Socket socket)
  {
    pSocket = &socket;
    while (socket.validState())
    {
      std::string msgString = readMsg(socket);
      if (msgString.length() == 0)
      {
        // invalid message
        break;
      }
      Message msg = Message::fromString(msgString);
      StaticLogger<1>::write("\n  -- " + clientHandlerName + " RecvThread read message: " + msg.name());
      //std::cout << "\n  -- " + clientHandlerName + " RecvThread read message: " + msg.name();
      if (msg.containsKey("file"))
      {
        receiveFile(msg);
      }
      pQ_->enQ(msg);
      //std::cout << "\n  -- message enqueued in rcvQ";
      if (msg.command() == "quit")
        break;
    }
    StaticLogger<1>::write("\n  -- terminating ClientHandler thread");
  }
private:
  BlockingQueue<Message>* pQ_;
  std::string clientHandlerName;
  Socket* pSocket = nullptr;
};

Comm::Comm(EndPoint ep, const std::string& name) : rcvr(ep, name), sndr(name), commName(name) {}

void Comm::start()
{
  BlockingQueue<Message>* pQ = rcvr.queue();
  ClientHandler* pCh = new ClientHandler(pQ, commName);
  /*
    There is a trivial memory leak here.  
    This ClientHandler is a prototype used to make ClientHandler copies for each connection.
    Those are not created on the heap, and are destroyed when the connection closes.
    Only one Client handler prototype is created for each Comm object and will live until
    the end of the program.

    I will clean this up in the next version.
  */
  rcvr.start(*pCh);
  sndr.start();
}

void Comm::stop()
{
  rcvr.stop();
  sndr.stop();
}

void Comm::postMessage(Message msg)
{
  sndr.postMessage(msg);
}

Message Comm::getMessage()
{
  return rcvr.getMessage();
}

std::string Comm::name()
{
  return commName;
}

//----< test stub >--------------------------------------------------

#ifdef TEST_COMM

/////////////////////////////////////////////////////////////////////
// Test #1 - Demonstrates Sender and Receiver operations

void DemoSndrRcvr(const std::string& machineName)
{
  SUtils::title("Demonstrating Sender and Receiver classes");

  SocketSystem ss;
  EndPoint ep1;
  ep1.port = 9091;
r()
{
  if (sendThread.joinable())
    sendThread.join();
}
//----< starts send thread deQ, inspect, and send loop >-------------

void Sender::start()
{
  std::function <void()> threadProc = [&]() {
    while (true)
    {
      Message msg = sndQ.deQ();

      if (msg.command() == "quit")
      {
        StaticLogger<1>::write("\n  -- send thread shutting down");
        return;
      }
      StaticLogger<1>::write("\n  -- " + sndrName + " send thread sending " + msg.name());
      std::string msgStr = msg.toString();

      if (msg.to().address != lastEP.address || msg.to().port != lastEP.port)
      {
        connecter.shutDown();
        //connecter.close();
        StaticLogger<1>::write("\n  -- attempting to connect to new endpoint: " + msg.to().toString());
        if (!connect(msg.to()))
        {
          StaticLogger<1>::write("\n can't connect");
          continue;
        }
        else
        {
          StaticLogger<1>::write("\n  connected to " + msg.to().toage: " << rcvdMsg.name();
  rcvdMsg.show();

  rcvdMsg = rcvr2.getMessage();  // blocks until message arrives
  StaticLogger<1>::flush();
  std::cout << "\n  rcvr2 in main received message: " << rcvdMsg.name();
  rcvdMsg.show();

  SUtils::title("Sending message to EndPoint that doesn't exist");

  msg.name("msg #3");
  msg.to(EndPoint("DoesNotExist", 1111));  // Unknown endpoint - should fail
  StaticLogger<1>::flush();
  std::cout << "\n  sndr in main posting message:  " << msg.name();
  msg.show();
  sndr.postMessage(msg);                   // will never reach rcvr

  msg.name("msg #4");
  msg.to(EndPoint("localhost", 9091));
  StaticLogger<1>::flush();
  std::cout << "\n  sndr in main posting message:  " << msg.name();
  sndr.postMessage(msg);                  // this should succeed
  StaticLogger<1>::flush();
  rcvdMsg = rcvr1.getMessage();
  std::cout << "\n  rcvr1 in main received message: " << rcvdMsg.name();
  rcvdMsg.show();

  rcvr1.stop();
  rcvr2.stop();
  sndr.stop()String());
        }
      }
      if (msg.containsKey("file"))
      {
        sendFile(msg);
      }
      else
      {
        bool sendRslt = connecter.send(msgStr.length(), (Socket::byte*)msgStr.c_str());
      }
    }
  };
  std::thread t(threadProc);
  sendThread = std::move(t);
}
//----< stops send thread by posting quit message >------------------

void Sender::stop()
{
  Message msg;
  msg.name("quit");
  msg.command("quit");
  postMessage(msg);
  connecter.shutDown();
}
//----< attempts to connect to endpoint ep >-------------------------

bool Sender::connect(EndPoint ep)
{
  lastEP = ep;
  return connecter.connect(ep.address, ep.port);
}
//----< posts message to send queue >--------------------------------

void Sender::postMessage(Message msg)
{
  sndQ.enQ(msg);
}
//----< sends binary file >------------------------------------------
/*
*  - not implemented yet
*/
bool Sender::sendFile(Message msg)
{
  if (!msg.containsKey("file"))
    return false;
  // send msg from comm1 to comm2
  msg.name("msg #2");
  msg.from(ep1);
  msg.to(ep2);
  StaticLogger<1>::flush();
  std::cout << "\n  comm1 in main posting message:  " << msg.name();
  comm1.postMessage(msg);
  msg = comm2.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main received message: " << msg.name();
  msg.show();

  // send msg from comm2 to comm1
  msg.name("msg #3");
  msg.to(ep1);
  msg.from(ep2);
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main posting message:  " << msg.name();
  comm2.postMessage(msg);
  msg = comm1.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm1 in main received message: " << msg.name();
  msg.show();

  // send msg from comm2 to comm2
  msg.name("msg #4");
  msg.from(ep2);
  msg.to(ep2);
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main posting message:  " << msg.name();
  comm2.postMessage(msg);
  msg = comm2.getMessage();
  StaticLogger<1>::flush();
  std::cout << "\n  comm2 in main received message: " << msg.name();
  msg.show();

  comm1.stop();
  comm2.stop();
  StaticLogger<1>::flush();
  std::cout << "\n  press enter to quit DemoComm";
  _getche();
}
/////////////////////////////////////////////////////////////////////
// Test #3 - Demonstrate server with two concurrent clients
//           sending and receiving messages

//----< handler for first concurrent client >------------------------

void ThreadProcClnt1()
{
  Comm comm(EndPoint("localhost", 9891), "client1Comm");
  comm.start();
  EndPoint serverEP("localhost", 9890);
  EndPoint clientEP("localhost", 9891);
  size_t IMax = 3;
  for (size_t i = 0; i < IMax; ++i)
  {
    Message msg(serverEP, clientEP);
    msg.name("client #1 : msg #" + Utilities::Converter<size_t>::toString(i));
    std::cout << "\n  " + comm.name() + " posting:  " << msg.name();
    comm.postMessage(msg);
    Message rply = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received: " << rply.name();
    ::Sleep(100);
  }
  Message fileMsg(serverEP, clientEP);
  fileMsg.name("fileSender");
  fileMsg.file("logger.cpp");
  comm.postMessage(fileMsg);
  ::Sleep(200);

  Message stop;
  stop.name("stop");
  stop.to(serverEP);
  stop.command("stop");
  comm.postMessage(stop);
}
//----< handler for 2nd concurrent client >--------------------------

void ThreadProcClnt2()
{
  Comm comm(EndPoint("localhost", 9892), "client2Comm");
  comm.start();
  EndPoint serverEP("localhost", 9890);
  EndPoint clientEP("localhost", 9892);
  size_t IMax = 3;
  for (size_t i = 0; i < IMax; ++i)
  {
    Message msg(serverEP, clientEP);
    msg.name("client #2 : msg #" + Utilities::Converter<size_t>::toString(i));
    std::cout << "\n  " + comm.name() + " posting:  " << msg.name();
    comm.postMessage(msg);
    Message rply = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received: " << rply.name();
  }
  Message fileMsg(serverEP, clientEP);
  fileMsg.name("fileSender");
  fileMsg.file("logger.h");
  comm.postMessage(fileMsg);
  ::Sleep(200);
}
//----< server demonstrates two-way asynchronous communication >-----
/*
*  - One server receiving messages and sending replies to
*    two concurrent clients.
*/
void DemoClientServer()
{
  SUtils::title("Demonstrating Client-Server - one server with two concurrent clients");

  SocketSystem ss;

  EndPoint serverEP("localhost", 9890);
  EndPoint clientEP("localhost", 9891);
  Comm comm(serverEP, "serverComm");
  comm.start();
  std::thread t1(ThreadProcClnt1);
  t1.detach();
  std::thread t2(ThreadProcClnt2);
  t2.detach();

  Message msg, rply;
  rply.name("reply");
  size_t count = 0;
  while (true)
  {
    // display each incoming message

    msg = comm.getMessage();
    std::cout << "\n  " + comm.name() + " received message: " << msg.name();

    if (msg.containsKey("file"))  // is this a file message?
    {
      if(msg.contentLength() == 0)
        std::cout << "\n  " + comm.name() + " received file \"" +---------------------------

  ~ClientHandler() 
  { 
    StaticLogger<1>::write("\n  -- ClientHandler destroyed;"); 
  }
  //----< set BlockingQueue >----------------------------------------

  void setQueue(BlockingQueue<Message>* pQ)
  {
    pQ_ = pQ;
  }
  //----< frame message string by reading bytes from socket >--------

  std::string readMsg(Socket& socket)
  {
    std::string temp, msgString;
    while (socket.validState())
    {
      temp = socket.recvString('\n');  // read attribute
      msgString += temp;
      if (temp.length() < 2)           // if empty line we are done
        break;
    }
    return msgString;
  }
  //----< receive file blocks >--------------------------------------
  /*
  *  - expects msg to contain file and contentLength attributes
  *  - expects to be connected to appropriate destination
  *  - these requirements are established in Sender::start()
  */
  bool receiveFile(Message msg)
  {
    std::string fileName = msg.file();
    std::st with your machine name
  //DemoCommClass("Odin");
  DemoClientServer();

  return 0;
}
#endif
