/*
 * ReceiverSocket.hxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#ifndef RECEIVERSOCKET_HXX_
#define RECEIVERSOCKET_HXX_

#include <list>
#include "Socket.hxx"

namespace mdm {
namespace mddproxy {

typedef std::list<Socket*> SenderListT;
using namespace std;

class ReceiverSocket: public Socket {
public:
	// Basically, for the MddProxy unless there is an output path we don't create the Receiver
	explicit ReceiverSocket(SenderListT* senderList, AddrT* addr);
	virtual ~ReceiverSocket();

	void SetSenderList(std::list<Socket*>* senderList);

	AddrT* Create();
	void ReceiveData();
	void SendData(const char*,size_t);
	void SetBufferSize(size_t newBufferSize);

private:

	SenderListT* senderSocketList;
	char* buff;
};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* RECEIVERSOCKET_HXX_ */
