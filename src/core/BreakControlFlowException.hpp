/// Ben H.D. Jones 2019

#pragma once

#include <exception>

namespace jasl {
	class BreakControlFlowException : std::exception {
	  public:
	  	virtual ~BreakControlFlowException() throw (){}
	  	virtual const char* what() const throw (){
	       return "break";
	    }
	};
}