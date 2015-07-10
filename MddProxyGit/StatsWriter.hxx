/*
 * StatsWriter.hxx
 *
 *  Created on: Jul 7, 2015
 *      Author: kheddar
 */

#ifndef STATSWRITER_HXX_
#define STATSWRITER_HXX_

#include "StatsCollector.hxx"
#include <map>
#include <pthread.h>

namespace mdm {
namespace mddproxy {

typedef std::map<int, StatsCollector*> StatsListT;
typedef StatsListT::iterator StatsListItT;

class StatsWriter {
public:
	StatsWriter();
	virtual ~StatsWriter();

	void* WriteStats();

	void Start(uint16_t seconds=300);
	void Stop();

	StatsCollector* GetStatsCollector(int socketId, const char* addr, int port);
	static void *RunHelper(void *context)
	{
		return ((StatsWriter *)context)->WriteStats();
	}

private:

	StatsListT* statsCollectors;
	pthread_t* thread;
	bool writeStats;
	uint16_t frequency;

};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* STATSWRITER_HXX_ */
