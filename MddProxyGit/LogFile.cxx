/*
 * LogFile.cxx
 *
 *  Created on: May 8, 2015
 *      Author: kheddar
 */

#include "LogFile.hxx"

namespace mdm {
namespace mddproxy {

LogFile::LogFile(std::string& path) {
	log = new std::ofstream( path.c_str(),
					std::ofstream::out|std::ofstream::ate|std::ofstream::app );

	pthread_mutex_init(&lock,NULL);
}

LogFile::~LogFile()
{
	if (log->is_open())
		log->close();

	delete log;
}

std::ofstream* LogFile::getLogger()
{
	pthread_mutex_lock(&lock);
	return log;
}

void LogFile::leave()
{
	pthread_mutex_unlock(&lock);
}

} /* namespace mddproxy */
} /* namespace mdm */
