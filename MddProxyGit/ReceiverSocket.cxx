/*
 * ReceiverSocket.cxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#include "ReceiverSocket.hxx"
#include "MddProxyException.hxx"
#include <boost/foreach.hpp>
#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#define MAX_PACKET_SIZE 1024
namespace mdm {
namespace mddproxy {

ReceiverSocket::ReceiverSocket(std::list<Socket*>* senderList, AddrT* addr): Socket(addr)
{
	senderSocketList = senderList;
	buff = new char[MAX_PACKET_SIZE];
}

ReceiverSocket::~ReceiverSocket()
{
	// JOIN multicast group on default interface
	struct ip_mreq imreq;

	imreq.imr_multiaddr.s_addr = socketObj->addr.sin_addr.s_addr;
	imreq.imr_interface.s_addr = socketObj->interface.sin_addr.s_addr;

	int retCode = setsockopt(socketObj->socket, IPPROTO_IP, IP_DROP_MEMBERSHIP,(const void *)&imreq, sizeof(struct ip_mreq));
	THROW_IF(retCode < 0, "Can't drop membership to interface %s err: %s", (const char*) inet_ntoa(socketObj->addr.sin_addr),
			 strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	delete buff;
}

void ReceiverSocket::SetSenderList(std::list<Socket*>* senderList)
{
	 senderSocketList = senderList;
}

void ReceiverSocket::ReceiveData()
{
	struct sockaddr_in srcAddress;
	socklen_t sockLen;
	size_t count;
	do
	{
		count = recvfrom(socketObj->socket, buff, MAX_PACKET_SIZE,0, (struct sockaddr *)&srcAddress, &sockLen);
		//count = read(socketObj->socket, buff, MAX_PACKET_SIZE);
		if ( count > 0 && count <= MAX_PACKET_SIZE)
			BOOST_FOREACH(SenderListT::value_type sender,*senderSocketList)
			{
				sender->SendData(buff, count);
			}
		else
			break;
	} while(count > 0);
}

void ReceiverSocket::SendData(void*,size_t)
{}


AddrT* ReceiverSocket::Create()
{
	struct ifaddrs *addrs, *iap;
	struct sockaddr_in *sa;
	bool intfUp = false;

	if (socketObj->interface.sin_addr.s_addr != INADDR_ANY)
	{
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
			THROW("[MULTICASTLIB] <%s:%u>, the receiver interface %s (inet addr:%s) is down",
				(const char*) inet_ntoa(socketObj->addr.sin_addr),
				(unsigned) ntohs(socketObj->addr.sin_port),
				(const char*) socketObj->interfaceName,
				(const char*) inet_ntoa(socketObj->interface.sin_addr));
		}
	}
	socketObj->socket = socket(socketObj->addr.sin_family, socketObj->protocol|SOCK_CLOEXEC|SOCK_NONBLOCK, 0);


	int option=1;
	if (setsockopt(socketObj->socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option)) < 0)
		THROW("Error setting socket option");

	struct sockaddr_in localSock;
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(socketObj->interface.sin_port);
	localSock.sin_addr.s_addr = INADDR_ANY;
	int retCode = bind (socketObj->socket, (struct sockaddr *)&localSock, sizeof(localSock));

	THROW_IF(retCode < 0, "Can't bind to interface %s err: %s", (const char*) inet_ntoa(socketObj->interface.sin_addr),
			strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	socklen_t optlen = sizeof(size_t);
	getsockopt(socketObj->socket, SOL_SOCKET, SO_RCVBUF, &socketBufferSize, &optlen);

	struct ip_mreq imreq;

	imreq.imr_multiaddr.s_addr = socketObj->addr.sin_addr.s_addr;
	imreq.imr_interface.s_addr = socketObj->interface.sin_addr.s_addr;

	// JOIN multicast group on default interface
	retCode = setsockopt(socketObj->socket, IPPROTO_IP, IP_ADD_MEMBERSHIP,(const void *)&imreq, sizeof(struct ip_mreq));
	THROW_IF(retCode < 0, "Can't Add membership to interface %s err: %s", (const char*) inet_ntoa(socketObj->addr.sin_addr),
			 strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));


	// Disable UDP Checksun
	retCode = setsockopt( socketObj->socket,        // socket
							SOL_SOCKET,                    // level
							SO_NO_CHECK,               // option is Multicast
							(void*)&option,
							sizeof(option) );
	THROW_IF(retCode < 0, "Can't set SO_NO_CHECK %s err: %s", (const char*) inet_ntoa(socketObj->addr.sin_addr),
				 strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	return socketObj;
}
void ReceiverSocket::SetBufferSize(size_t newBufferSize)
{
	if (newBufferSize > 0 )
	{
		int retCode = setsockopt( socketObj->socket, SOL_SOCKET, SO_RCVBUF,(void*)&newBufferSize, sizeof(size_t) );
		THROW_IF(retCode < 0, "Setting SO_RCVBUF error address %s", (const char*) inet_ntoa(socketObj->addr.sin_addr));

		Socket::SetBufferSize(newBufferSize);
	}
}
} /* namespace mddproxy */
} /* namespace mdm */
