/*
 * StatsWriter.cxx
 *
 *  Created on: Jul 7, 2015
 *      Author: kheddar
 */

#include "StatsWriter.hxx"
#include "MddProxyRunLogger.hxx"
#include <boost/foreach.hpp>
#include "MddProxyException.hxx"

namespace mdm {
namespace mddproxy {

StatsWriter::StatsWriter()
{
	statsCollectors = new StatsListT;
	thread = 0;
	writeStats = false;
	frequency = 0;
}

StatsWriter::~StatsWriter()
{
	BOOST_FOREACH(StatsListT::value_type stats, *statsCollectors)
	{
		delete stats.second;
	}
	statsCollectors->clear();
	delete statsCollectors;
}

StatsCollector* StatsWriter::GetStatsCollector(int socketId, const char* addr, int port )
{
	StatsCollector* newStats = new StatsCollector(addr, port);
	statsCollectors->insert(std::make_pair(socketId,newStats));

	return newStats;
}

void* StatsWriter::WriteStats()
{
	while(writeStats)
	{
		sleep(frequency);
		LOGINF("Writing stats every invtl<%u>", frequency);
		BOOST_FOREACH(StatsListT::value_type stats, *statsCollectors)
		{
			StatsRateT statsInPrevPeriod = stats.second->GetStatsInPrevPeriod();
			LOGINF("<%s:%u>, rate <%u> bytes <%u>",
							stats.second->addr.c_str(),
							stats.second->port,
							statsInPrevPeriod.rate, statsInPrevPeriod.bytes);
		}
	}

	pthread_exit(NULL);

	return (void*)NULL;
}
void StatsWriter::Start(uint16_t seconds)
{
	writeStats = true;
	frequency = seconds;
	thread = new pthread_t;
	int retCode = pthread_create(thread, NULL, &StatsWriter::RunHelper, (void*)this);
	if (retCode != 0 )
		THROW("Can't create thread");

	pthread_join(*thread, NULL);
}
void StatsWriter::Stop()
{
	writeStats = false;
}

} /* namespace mddproxy */
} /* namespace mdm */
