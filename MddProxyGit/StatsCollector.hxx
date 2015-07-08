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


namespace mdm {
namespace mddproxy {

typedef struct
{
	uint32_t rate;
	uint32_t bytes;
} StatsRateT;

class StatsCollector {
public:
	StatsCollector();
	virtual ~StatsCollector();

	StatsRateT GetStatsInPrevPeriod();

	void ReportBytes(size_t );

	void Start();
	void Stop();

private:

	uint32_t intvlBytesTrns;
	timeval intvlStart;
	uint64_t totalBytesTrns;
	timeval prevReportingTimestamp;
};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* STATSCOLLECTOR_HXX_ */
