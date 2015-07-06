/*
 * CommandLineInterface.hxx
 *
 *  Created on: May 29, 2015
 *      Author: kheddar
 */

#ifndef COMMANDLINEINTERFACE_HXX_
#define COMMANDLINEINTERFACE_HXX_

#include "Socket.hxx"
#include "MddProxy.hxx"



namespace mdm {
namespace mddproxy {



class CommandLineInterface : public Socket
{
public:
	CommandLineInterface(AddrT* addr, MddProxy* mddproxy);
	virtual ~CommandLineInterface();

	char* CreateResponse();
	virtual AddrT* Create();
	virtual void ReceiveData();
	virtual void SendData(void*,size_t);

	void Listen();

private:
	MddProxy* proxy;

	int clientSocket;

};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* COMMANDLINEINTERFACE_HXX_ */
