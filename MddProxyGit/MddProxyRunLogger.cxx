/*
 * MddProxyRunLogger.cxx
 *
 *  Created on: May 7, 2015
 *      Author: kheddar
 */

#include "MddProxyRunLogger.hxx"
#include <cstdarg>

namespace mdm {
namespace mddproxy {

Logger::Logger(std::string& filePath, std::string& filename):
		absolutepath(filePath + filename),
			logFile(absolutepath)
{
	logString = new char[MAX_LOG_SIZE];
}

Logger::~Logger()
{
	delete logString;
}

void Logger::TruncateLog()
{
	Open(true);
}

bool Logger::Open(bool truncate)
{
	return false;
}

void Logger::Log(int line,
		const char *file,
		const char *func,
		const char *errorStr,
		...)
{
	//const char* str = const_cast<char*>(logString);
	char* str = new char[2048];
	sprintf( logString, "%d\t%s\t%s", line, file, func);
	logFile.getLogger()->write(str, 256);

	va_list pa;

	va_start( pa, errorStr );
	vsnprintf(logString, MAX_LOG_SIZE-250, errorStr, pa );
	perror(logString);
	va_end( pa );

	logFile.getLogger()->write(str, MAX_LOG_SIZE-250);
}

void Logger::LogException(int line,
		const char *file,
		const char *func,
		const char *errorStr)
{
	sprintf( logString, "%d\t%s\t%s\t%s", line, file, func,errorStr );
	logFile.getLogger()->write(logString, MAX_LOG_SIZE);
}
} /* namespace mddproxy */
} /* namespace mdm */
