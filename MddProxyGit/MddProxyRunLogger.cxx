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
	timeval currTime = CommonFunctions::GetTime();

	localtime_r(&currTime.tv_sec, &nowtm);
	strftime(tmbuf, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", &nowtm);

	sprintf( logString, "%s.%06ld\t%d\t%s\t%s\t%s\t",
				tmbuf, currTime.tv_usec,
				line, file, func, GetLogLevelString(logLevel).c_str());

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
	localtime_r(&currTime.tv_sec, &nowtmErr);
	strftime(tmbufErr, sizeof tmbuf, "%Y-%m-%d %H:%M:%S", &nowtmErr);

	sprintf( logString, "%s.%06ld\t%d\t%s\t%s\t%s",
			tmbufErr, currTime.tv_usec,
			line, file, func,errorStr );

	std::ofstream* writer = logFile.getLogger();
	writer->write(logString, strlen(logString));
	writer->write("\n",1);
	logFile.leave();

	writer->flush();
}

} /* namespace mddproxy */
} /* namespace mdm */
