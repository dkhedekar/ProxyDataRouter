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


namespace mdm {
namespace mddproxy {

class Logger {
public:
	explicit Logger(std::string& filePath, std::string& filename);
	virtual ~Logger();


	void TruncateLog();

	void Log(int line,
			const char *file,
			const char *func,
			const char *errorStr,
			...);

	void LogException(int line,
			const char *file,
			const char *func,
			const char *errorStr);

private:
	std::string absolutepath;
	LogFile logFile;
	char* logString;

	bool Open(bool truncate = false);
};

extern Logger* LoggerInstance;

#define LOG( exceptionStr, ... )\
{\
    LoggerInstance->Log( __LINE__,\
					__FILE__,\
					__func__,\
					exceptionStr,\
					##__VA_ARGS__ );\
}

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* MDDPROXYRUNLOGGER_HXX_ */
