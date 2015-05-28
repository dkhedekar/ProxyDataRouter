/*
 * MddProxyException.h
 *
 *  Created on: May 7, 2015
 *      Author: kheddar
 */

#ifndef MDDPROXYEXCEPTION_H_
#define MDDPROXYEXCEPTION_H_

#include <exception>
#include <cstdarg>

namespace mdm {
namespace mddproxy {

class MddProxyException;

#define THROW( exceptionStr, ... )\
{\
    throw MddProxyException( __LINE__,\
                        __FILE__,\
                        __func__,\
                        exceptionStr,\
                        ##__VA_ARGS__ );\
}

#define THROW_IF( cond, exceptionStr, ... )\
{\
	if ( cond )	\
    	throw MddProxyException( __LINE__,\
    			__FILE__,\
    			__func__,\
    			exceptionStr,\
    			##__VA_ARGS__ );\
}

#define MAX_EXCEPTION_LENGTH 1024

class MddProxyException: public std::exception
{
public:
	MddProxyException(int line,
			const char *file,
			const char *func,
			const char *errorStr,
			... )__attribute__((format( printf, 5, 6 )));


	virtual ~MddProxyException() throw() {}

private:
	char formattedMsg[MAX_EXCEPTION_LENGTH];


};

//@ TODO incorporate the exception code/severity
//@ TODO do we want to have CRITICAL exception so that the program stops?

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* MDDPROXYEXCEPTION_H_ */
