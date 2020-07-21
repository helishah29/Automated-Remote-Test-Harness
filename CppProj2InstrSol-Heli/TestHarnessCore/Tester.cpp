/////////////////////////////////////////////////////////////////////
// Tester.cpp - Prototype for Test Harness child Tester            //
// ver 1.0                                                         //
// heli shah, CSE687 - Object Oriented Design, Fall 2018      //
/////////////////////////////////////////////////////////////////////

// Tester is the Child Process for Project 3

#include "../CppCommWithFileXfer/MsgPassingComm/Comm.h"
#include "Tester.h"
#include "../Dll_Loader_With_Hosted_Resource/ITests.h"
#include "../Utilities/TestUtilities/TestUtilities.h"
#include "../Utilities/FileSystem/FileSystem.h"
#include "../Utilities/FileUtilities/FileUtilities.h"
#include "../Utilities/StringUtilities/StringUtilities.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <memory>
#include <vector>
#include <string.h>

#include <windows.h>

#include "../CppCommWithFileXfer/Message/Message.h"


using namespace MsgPassingCommunication;
using namespace Testing;
namespace Testing
{
	//----< initialize paths and logger >------------------------------

	Tester::Tester()
	{
		requestPath("../TestRequests");
		libPath("../TestLibraries");
		pLog(Utilities::Logger<0, Utilities::Lock>::getInstance());
		//pLog()->setTerminator("");
	}
	//----< deallocate logger >----------------------------------------

	Tester::~Tester()
	{
		delete pLog();
	}
	//----< load Test Requests >-----------------------------------------
	/*
	* - In Project #4 serialized TestRequests, sent by client, will be
	*   extracted from Comm channel.
	*/
	void Tester::loadTestRequests(TestRequest& tr)
	{
		std::vector<std::string> files = FileSystem::Directory::getFiles(requestPath());
		std::ifstream in;
		for (auto file : files)
		{
			if (!Utilities::openReadFile(requestPath() + "\\" + file, in))
				continue;
			std::ostringstream sstr;
			sstr << in.rdbuf();
			requests.valueRef().push_back(tr);
		//	pLog()->write("\n  loaded TestRequest \"" + tr.name() + "\"");
		//	pLog()->write("\n  " + tr.toString() + "\n");
		}
	}
	//----< load libraries >---------------------------------------------
	/*
	* - Loads all libraries specified in a TestRequest from libPath.
	* - Uses DllLoader member, dllLoader_, to load dlls and extract
	*   their tests.
	*/
	void Tester::loadLibraries(TestRequest& tr)
	{
		std::string path = FileSystem::Path::getFullFileSpec(libPath());
	//	pLog()->write("\n  loading from \"" + path + "\"");
		std::cout << "\nPrinting path:" << path;
		std::vector<std::string> files;
		for (Dll dll : tr.request.valueRef())
		{
			files.push_back(dll);
		}

		for (auto file : files)
		{
			//pLog()->write("\n    " + file);
			std::cout << std::endl << file;
			std::string fileSpec = path + "\\" + file;
			//fileSpec = fileSpec.substr(0, fileSpec.size() - 1);
			std::cout << "\nPrinting filespec inside loadLobraries:" << fileSpec;
			dllLoader_.loadAndExtract(fileSpec);
		}
		//pLog()->write("\n");
	}
	//----< executes tests >--------------------------
	/*
	* - Executes tests held by DllLoader instance,
	*   using TestUtilities infrastructure.
	*/
	bool Tester::doTests()
	{
		using sPtr = std::shared_ptr<ITest>;
		Utilities::TestExecutive te;
		for (ITest* test : dllLoader_.tests()) {
			// Host passes logger resource pointer to test
			// - test will use logger via this pointer
			test->acceptHostedResource(pLog.valueRef());
			sPtr pTest(test);
			te.registerTest(pTest, pTest->name());
		}
		bool result = te.doTests();
		return result;
	}
}
void sndMsgs(Comm& comm, EndPoint epTo, EndPoint epFrom, int result)
{
	std::cout << "\n" << "Child sending message" << "\n";
	EndPoint eptomother("localhost", 8082);
	EndPoint eptoclient("localhost", 8081);

	MsgPassingCommunication::Message msg,msg2;
	msg.to(eptomother);
	msg.from(epFrom);
	msg.attribute("childReady"," kiddo");
	comm.postMessage(msg);
	std::string res;
	msg2.to(eptoclient);
	msg2.from(epFrom);
	msg2.name("test result");
	if (result)
		res = "passed";
	else
		res = "failed";
	msg2.attribute("result", res);
	
	comm.postMessage(msg2);

}
void getMsgs(Comm& comm)
{
	Tester tester;
	MsgPassingCommunication::Message msg;
	while (true)
	{
		msg = comm.getMessage();
		TestRequest tr = TestRequest::fromString(msg.value("TestRequest"));
		tester.clear();

		tester.loadTestRequests(tr);
		std::cout << "\n  CHILD recvd message \"" << msg.name() << "\"";
		std::cout << "\n Child Executing test...";
		tester.loadLibraries(tr);
		int result = tester.doTests();
		std::cout << result << std::endl;
		std::cout << std::endl << "Name :" << msg.name() << "\t" << "Value in attribute:" << msg.value("TestRequest") << std::endl;

		//std::this_thread::sleep_for(std::chrono::seconds(3));
		sndMsgs(comm, msg.from(), msg.to(), result);

		if (msg.command() == "stop")
			break;
	}
}

int main(int argc,char* argv[])
{
	//std::cout << argv[1]<<std::endl;
	int temp = std::stoi(argv[1]);
	EndPoint epPeer1("localhost", 8082);
	EndPoint epPeer2("localhost", temp);
	Comm comm(epPeer2, "Child");
	comm.start();
	MsgPassingCommunication::Message msg, msg2;
	msg.to(epPeer1);
	msg.from(epPeer2);
	msg.attribute("childReady", "");
	comm.postMessage(msg);

	std::function<void()> tproc = [&]() { getMsgs(comm); };
	std::thread tget(tproc);
	tget.detach();

	std::cout << Utilities::sTitle("Testing Tester using Dll Loader and Logger");
	// create ready message here.
	TestRequest tr;
	Tester tester;

	std::cin.get();
	//comm.stop();
	Utilities::putline(2);
	return 0;

}
