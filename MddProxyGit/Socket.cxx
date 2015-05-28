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

Socket::Socket(AddrT* addr): socketObj(addr), socketBufferSize(0)
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
} /* namespace mddproxy */
} /* namespace mdm */
