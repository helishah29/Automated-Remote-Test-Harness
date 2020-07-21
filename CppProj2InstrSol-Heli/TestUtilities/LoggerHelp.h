#pragma once
/////////////////////////////////////////////////////////////////////
// LoggerHelp.h - demonstrates how to register streams             //
//                                                                 //
// Jim Fawcett, CSE687 - Object Oriented Design, Fall 2018         //
/////////////////////////////////////////////////////////////////////
/*
*  What's being demonstrated:
*  - std::ostream is a base class for output streams that include
*    std::ofstream, used to write to files, and std::ostringstream,
*    used to write to in-memory strings using the std::ostream
*    interface.
*  - The most fundamental principle in Object Oriented Design is the
*    Liskov Substitution Principle (LSP), which states:
*
*      Functions that accept base class pointers or references also
*      accept pointers or references to any class that derives from
*      the base, and can be used by the function without any special
*      knowledge beyond the base class interface.
*
*  That means that we can register standard streams that derive from
*  std::ostream with a logger if its add function accepts pointers or
*  references to the base std::ostream class.
*/
#include <iostream>
#include <memory>
#include <vector>

class LoggerHelp
{
public:
  void addStream(std::ostream* pStream)
  {
    streams_.push_back(pStream);
  }
  void write(const std::string& text)
  {
    for (auto pStrm : streams_)
      *pStrm << text.c_str();
  }
  void wordclear() {
	  streams_.clear();
  }
private:
  std::vector<std::ostream*> streams_;
};