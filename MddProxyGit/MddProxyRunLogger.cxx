/*
 * MddProxyRunLogger.cxx
 *
 *  Created on: May 7, 2015
 *      Author: kheddar
 */

#include "MddProxyRunLogger.hxx"
#include "CommonFunctions.hxx"

#include <cstdarg>
#include <string.h>


namespace mdm {
namespace mddproxy {

Logger::Logger(std::string& filePath, std::string& filename):
		absolutepath(filePath + filename),
			logFile(absolutepath),
			logLevel(INFORMATIONAL)
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

void Logger::SetLogLevel(LogLevelT newLogLevel)
{
	logLevel = newLogLevel;
}

void Logger::Log(int line,
		LogLevelT logLevel,
		const char *file,
		const char *func,
		const char *errorStr,
		...)
{
	std::string logLevelString = logLevel == INFORMATIONAL ? "INFO" : "DEBUG";
	timeval currTime = CommonFunctions::GetTime();
	sprintf( logString, "%ld\t%d\t%s\t%s\t%s\t",
			currTime.tv_sec * 1000000 + currTime.tv_usec,
			line, file, func, logLevelString.c_str());
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
	timeval currTime = CommonFunctions::GetTime();
	sprintf( logString, "%ld\t%d\t%s\t%s\t%s",
			currTime.tv_sec * 1000000 + currTime.tv_usec,
			line, file, func,errorStr );
	logFile.getLogger()->write(logString, strlen(logString));
	logFile.leave();
}
} /* namespace mddproxy */
} /* namespace mdm */
