/*
 * MddProxyRunLogger.cxx
 *
 *  Created on: May 7, 2015
 *      Author: kheddar
 */

#include "MddProxyRunLogger.hxx"
#include <cstdarg>
#include <string.h>

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
	sprintf( logString, "%d\t%s\t%s\t", line, file, func);
	std::ofstream* writer = logFile.getLogger();
	int hdrsz = strlen(logString);

	writer->write(logString, hdrsz);
	va_list pa;

	va_start( pa, errorStr );
	vsnprintf(logString, MAX_LOG_SIZE-hdrsz, errorStr, pa );
	perror(logString);
	va_end( pa );

	writer->write(logString, strlen(logString));
	writer->write("\n",1);
	logFile.leave();

	writer->flush();
}

void Logger::LogException(int line,
		const char *file,
		const char *func,
		const char *errorStr)
{
	sprintf( logString, "%d\t%s\t%s\t%s", line, file, func,errorStr );
	logFile.getLogger()->write(logString, strlen(logString));
	logFile.leave();
}
} /* namespace mddproxy */
} /* namespace mdm */
