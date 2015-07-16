/*
 * Socket.hxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#ifndef SOCKET_HXX_
#define SOCKET_HXX_

#include <netinet/in.h>
#include "CommonDefinitions.hxx"
#include "StatsCollector.hxx"

#define MAX_INTF_NAME 10
#define ERROR_BUFF_SIZE 1024

namespace mdm {
namespace mddproxy {

typedef struct
{
	struct sockaddr_in addr;
	enum __socket_type protocol;
	BroadcastT type;
	struct sockaddr_in interface;
	char interfaceName[MAX_INTF_NAME];
	int socket;
} AddrT;

class Socket {

public:
	explicit Socket(AddrT* addr);
	virtual ~Socket();

	virtual AddrT* Create() = 0;
	virtual void Close();
	virtual void ReceiveData() = 0;
	virtual void SendData(const char*,size_t) = 0;

	virtual void SetBufferSize(size_t newBufferSize);

	void SetStatsCollector(StatsCollector*);

protected:
	AddrT* socketObj;
	size_t socketBufferSize;

	StatsCollector* statsCollector;


	char errBuffer[ERROR_BUFF_SIZE];
};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* SOCKET_HXX_ */
