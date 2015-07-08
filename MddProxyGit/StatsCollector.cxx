/*
 * StatsCollector.cxx
 *
 *  Created on: Jul 7, 2015
 *      Author: kheddar
 */

#include "StatsCollector.hxx"
#include "MddProxyRunLogger.hxx"
#include "CommonFunctions.hxx"

namespace mdm {
namespace mddproxy {

StatsCollector::StatsCollector()
{
	intvlBytesTrns = totalBytesTrns = 0 ;
}

StatsCollector::~StatsCollector()
{}

void StatsCollector::Start()
{
	intvlBytesTrns = 0;
	prevReportingTimestamp = CommonFunctions::GetTime();
}

void StatsCollector::Stop()
{
	intvlBytesTrns = 0;
}

void StatsCollector::ReportBytes(size_t transferedBytes)
{
	intvlBytesTrns += transferedBytes;
}

StatsRateT StatsCollector::GetStatsInPrevPeriod()
{
	StatsRateT stats;

	stats.bytes = intvlBytesTrns;
	timeval currTime = CommonFunctions::GetTime();
	stats.rate = stats.bytes/(currTime.tv_sec - intvlStart.tv_sec );

	intvlStart = currTime;

	totalBytesTrns += intvlBytesTrns;
	intvlBytesTrns = 0;

	return stats;
}
} /* namespace mddproxy */
} /* namespace mdm */
