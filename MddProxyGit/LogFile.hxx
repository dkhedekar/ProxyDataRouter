/*
 * LogFile.hxx
 *
 *  Created on: May 8, 2015
 *      Author: kheddar
 */

#ifndef LOGFILE_HXX_
#define LOGFILE_HXX_

#include <fstream>
#include <pthread.h>

namespace mdm {
namespace mddproxy {

class LogFile {
public:
	explicit LogFile(std::string& path);
	virtual ~LogFile();

	std::ofstream* getLogger();
	void leave();

private:
	std::ofstream* log;
	pthread_mutex_t lock;

};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* LOGFILE_HXX_ */
