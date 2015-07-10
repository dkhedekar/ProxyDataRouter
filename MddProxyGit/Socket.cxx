/*
 * Socket.cxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#include "Socket.hxx"
#include <unistd.h>

namespace mdm {
namespace mddproxy {

Socket::Socket(AddrT* addr):
		socketObj(addr),
		socketBufferSize(0),
		statsCollector(NULL)
{}

Socket::~Socket()
{}

void Socket::Close()
{
	close(socketObj->socket);
}

void Socket::SetBufferSize(size_t newBufferSize)
{
	socketBufferSize = newBufferSize;
}

void Socket::SetStatsCollector(StatsCollector* stats)
{
	statsCollector = stats;
}
} /* namespace mddproxy */
} /* namespace mdm */
