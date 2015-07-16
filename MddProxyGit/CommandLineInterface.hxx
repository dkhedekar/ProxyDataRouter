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

#include <boost/property_tree/json_parser.hpp>
#include <streambuf>

namespace pt = boost::property_tree;
using namespace pt;

namespace mdm {
namespace mddproxy {


class CommandLineInterface : public Socket
{
	const ptree& empty_ptree()
	{
		static ptree t;
		return t;
	}

	struct membuf : std::streambuf
	{
	    membuf(char* begin, char* end) {
	        this->setg(begin, begin, end);
	    }
	};
public:
	CommandLineInterface(AddrT* addr, MddProxy* mddproxy);
	virtual ~CommandLineInterface();

	char* CreateResponse();
	virtual AddrT* Create();
	virtual void SendData(const char*,size_t);
	virtual void ReceiveData();
	void Listen();

private:
	MddProxy* proxy;
	int clientSocket;

	bool stopCommandIntf;
	std::string ProcessCommand(char* buffer, size_t inpBufferLen);


};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* COMMANDLINEINTERFACE_HXX_ */
