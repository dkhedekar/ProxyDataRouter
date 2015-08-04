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
	DEBUG=1,
	ERROR=2
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
	inline bool IsDebug();
	inline std::string GetLogLevelString(LogLevelT logLevel);
private:
	std::string absolutepath;
	LogFile logFile;
	char* logString;
	LogLevelT logLevel;
	char tmbuf[64];
	char tmbufErr[64];
	struct tm nowtm;
	struct tm nowtmErr;
	bool Open(bool truncate = false);
};


inline bool Logger::IsDebug()
{ return logLevel == DEBUG; }

inline std::string Logger::GetLogLevelString(LogLevelT logLevel)
{
	switch(logLevel)
	{
		case INFORMATIONAL: return "INFO"; break;
		case ERROR: return "ERROR"; break;
		default:
		case DEBUG: return "DEBUG"; break;
	}
	return "DEBUG";
}

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

#define LOGERR( str, ... )\
{\
    LoggerInstance->Log( __LINE__,\
    				ERROR,\
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
