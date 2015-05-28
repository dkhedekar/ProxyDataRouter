/*
 * MddProxyException.cpp
 *
 *  Created on: May 7, 2015
 *      Author: kheddar
 */

#include "MddProxyException.hxx"
#include "CommonDefinitions.hxx"
#include "MddProxyRunLogger.hxx"
#include <stdio.h>
#include <stdarg.h>

namespace mdm {
namespace mddproxy {

extern Logger* Instance;

MddProxyException::MddProxyException(int line,
		const char *file,
		const char *func,
		const char *errorStr,
		... )
{
	va_list pa;
	va_start( pa, errorStr );
	vsnprintf(formattedMsg, MAX_EXCEPTION_LENGTH, errorStr, pa );
	perror(formattedMsg);

	va_end( pa );

	Instance->LogException(line,file,func, formattedMsg);

}

} /* namespace mddproxy */
} /* namespace mdm */
