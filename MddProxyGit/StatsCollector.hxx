/*
 * StatsCollector.hxx
 *
 *  Created on: Jul 7, 2015
 *      Author: kheddar
 */

#ifndef STATSCOLLECTOR_HXX_
#define STATSCOLLECTOR_HXX_

#include <stdint.h>
#include <time.h>
#include <sys/time.h>
#include <string>

const size_t MAX_REPORTED_ADDR_SIZE=20;

namespace mdm {
namespace mddproxy {

typedef struct
{
	uint32_t rate;
	uint32_t bytes;
} StatsRateT;

class StatsCollector {

	friend class StatsWriter;
public:

	StatsCollector(const char* socketAddr, int socketPort);
	virtual ~StatsCollector();
	const StatsRateT GetStatsInPrevPeriod();
	void ReportBytes(size_t );
	void Start();
	void Stop();

private:

	uint32_t intvlBytesTrns;
	timeval intvlStart;
	uint64_t totalBytesTrns;
	timeval prevReportingTimestamp;

	std::string addr;
	int port;
};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* STATSCOLLECTOR_HXX_ */
