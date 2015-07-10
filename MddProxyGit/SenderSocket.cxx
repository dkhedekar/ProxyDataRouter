/*
 * SenderSocket.cxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#include "SenderSocket.hxx"
#include "MddProxyException.hxx"
#include "MddProxyRunLogger.hxx"
#include <string.h>
#include <errno.h>
#include <boost/foreach.hpp>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>

namespace mdm {
namespace mddproxy {

SenderSocket::SenderSocket(AddrT* addr): Socket(addr), isLoopBackEnabled(false)
{
}

SenderSocket::~SenderSocket()
{
}

void SenderSocket::ReceiveData()
{}

void SenderSocket::SendData(void* buff,size_t length)
{
	if (sendto(socketObj->socket, buff, length, 0, (struct sockaddr*)&socketObj->addr, sizeof(struct sockaddr) ) < 0)
		THROW( "Can't send data for address %s %s", (const char*) inet_ntoa(socketObj->addr.sin_addr), strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	statsCollector->ReportBytes(length);
}
void SenderSocket::EnableLoopback()
{
	isLoopBackEnabled = true;
}
void SenderSocket::DisableLoopback()
{
	isLoopBackEnabled = false;
}


AddrT* SenderSocket::Create()
{
	struct ifaddrs *addrs, *iap;
	struct sockaddr_in *sa;
	bool intfUp = false;

	getifaddrs(&addrs);
	for (iap = addrs; iap != NULL; iap = iap->ifa_next)
	{
		if (iap->ifa_addr && iap->ifa_addr->sa_family == AF_INET)
		{
			sa = (struct sockaddr_in *)(iap->ifa_addr);
			if (sa->sin_addr.s_addr == socketObj->interface.sin_addr.s_addr)
			{
				intfUp = iap->ifa_flags & IFF_UP;
				memcpy(socketObj->interfaceName, iap->ifa_name, MAX_INTF_NAME);
				break;
			}
		}
	}
	freeifaddrs(addrs);

	if (!intfUp)
	{
		THROW("<%s:%u>, the receiver interface %s (inet addr:%s) is down",
			(const char*) inet_ntoa(socketObj->addr.sin_addr),
			(unsigned) ntohs(socketObj->addr.sin_port),
			(const char*) socketObj->interfaceName,
			(const char*) inet_ntoa(socketObj->interface.sin_addr));
	}

	socketObj->socket = socket(socketObj->addr.sin_family, socketObj->protocol |SOCK_CLOEXEC|SOCK_NONBLOCK, 0);

	// Set socket options for multi-cast socket
	// Set the outgoing interface for IP and Multicast
	int retCode = setsockopt(socketObj->socket, IPPROTO_IP, IP_MULTICAST_IF, &socketObj->interface.sin_addr, sizeof(struct in_addr));

	char addr[100];
	strcpy(addr, inet_ntoa(socketObj->addr.sin_addr));
	const char* intfaddr = inet_ntoa(socketObj->interface.sin_addr);

	THROW_IF( retCode<0,
		"Error in setsockopt while setting outgoing interface for address=<%s:%u> (inet addr:%s) %s",
		addr,
		(unsigned) ntohs(socketObj->addr.sin_port),
		intfaddr,
		strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));


	// Set multicast packet TTL
	unsigned char ttlValue = 10;
	retCode = setsockopt(socketObj->socket, IPPROTO_IP, IP_MULTICAST_TTL, &ttlValue, sizeof(unsigned char));
	THROW_IF( retCode<0,
			"Error in setsockopt while setting TTL for address=<%s:%u> %s",
			(const char*) inet_ntoa(socketObj->addr.sin_addr),
			(unsigned) ntohs(socketObj->addr.sin_port),
			strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	unsigned char one = isLoopBackEnabled ? 1 : 0;
	retCode = setsockopt(socketObj->socket, IPPROTO_IP, IP_MULTICAST_LOOP, &one, sizeof(unsigned char));
	THROW_IF( retCode<0,
				"Error in setsockopt while setting loopback for address=<%s:%u> %s",
				(const char*) inet_ntoa(socketObj->addr.sin_addr),
				(unsigned) ntohs(socketObj->addr.sin_port),
				strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	socklen_t optlen = sizeof(size_t);
	getsockopt(socketObj->socket, SOL_SOCKET, SO_SNDBUF, &socketBufferSize, &optlen);

	LOGINF("created sender: <%s:%u> %s (inet addr:%s)",
					(const char*) inet_ntoa(socketObj->addr.sin_addr),
					(unsigned) ntohs(socketObj->addr.sin_port),
					(const char*) socketObj->interfaceName,
					(const char*) inet_ntoa(socketObj->interface.sin_addr));

	return socketObj;
}

void SenderSocket::SetBufferSize(size_t newBufferSize)
{
	if (newBufferSize > 0 )
	{
		int retCode = setsockopt( socketObj->socket, SOL_SOCKET, SO_SNDBUF,(void*)&newBufferSize, sizeof(size_t) );
		THROW_IF(retCode < 0, "Setting SO_SNDBUF error address %s", (const char*) inet_ntoa(socketObj->addr.sin_addr));

		Socket::SetBufferSize(newBufferSize);
	}

	LOGINF("set send buffer size to (%d): <%s:%u> %s (inet addr:%s)",
					newBufferSize,
					(const char*) inet_ntoa(socketObj->addr.sin_addr),
					(unsigned) ntohs(socketObj->addr.sin_port),
					(const char*) socketObj->interfaceName,
					(const char*) inet_ntoa(socketObj->interface.sin_addr));
}

} /* namespace mddproxy */
} /* namespace mdm */
