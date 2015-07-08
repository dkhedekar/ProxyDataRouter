/*
 * CommonFunctions.hxx
 *
 *  Created on: Jul 8, 2015
 *      Author: kheddar
 */

#ifndef COMMONFUNCTIONS_HXX_
#define COMMONFUNCTIONS_HXX_

#include <time.h>
#include <sys/time.h>

namespace mdm {
namespace mddproxy {

class CommonFunctions {
public:
	static timeval GetTime()
	{
		struct timeval currtime;
		gettimeofday(&currtime, NULL);
		return currtime;
	}

};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* COMMONFUNCTIONS_HXX_ */
