#ifndef _GUM_EXCEPTION_H_
#define _GUM_EXCEPTION_H_

#include <cu/cu_stl.h>

#include <exception>

namespace gum
{

class Exception : public std::exception
{
public:
	Exception(const CU_STR& msg);
	Exception(const char* fmt, ...);
	virtual ~Exception() throw() {}

	virtual const char* what() const throw() { 
		return m_message.c_str(); 
	}

private:
	CU_STR m_message;

}; // Exception 

}

#endif // _GUM_EXCEPTION_H_
