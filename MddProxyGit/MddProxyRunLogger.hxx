/*
 * MddProxyRunLogger.hxx
 *
 *  Created on: May 7, 2015
 *      Author: kheddar
 */

#ifndef MDDPROXYRUNLOGGER_HXX_
#define MDDPROXYRUNLOGGER_HXX_

#include <string>
#include "LogFile.hxx"
#include "CommonDefinitions.hxx"

const long MAX_LOG_SIZE = 2048;

typedef enum
{
	INFORMATIONAL=0,
	DEBUG=1
}LogLevelT;

namespace mdm {
namespace mddproxy {

class Logger {
public:
	explicit Logger(std::string& filePath, std::string& filename);
	virtual ~Logger();


	void TruncateLog();

	void Log(int line,
			LogLevelT logLevel,
			const char *file,
			const char *func,
			const char *errorStr,
			...);

	void LogException(int line,
			const char *file,
			const char *func,
			const char *errorStr);

	void SetLogLevel(LogLevelT);

	LogLevelT GetLogLevel() { return logLevel; }
	bool IsDebug() { return logLevel == DEBUG; }

private:
	std::string absolutepath;
	LogFile logFile;
	char* logString;
	LogLevelT logLevel;

	bool Open(bool truncate = false);
};

extern Logger* LoggerInstance;

#define LOGINF( str, ... )\
{\
    LoggerInstance->Log( __LINE__,\
    				INFORMATIONAL,\
					__FILE__,\
					__func__,\
					str,\
					##__VA_ARGS__ );\
}

#define LOGDEB( str, ... )\
{\
	if (LoggerInstance->IsDebug())\
    	LoggerInstance->Log( __LINE__,\
    				DEBUG,\
					__FILE__,\
					__func__,\
					str,\
					##__VA_ARGS__ );\
}

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* MDDPROXYRUNLOGGER_HXX_ */
