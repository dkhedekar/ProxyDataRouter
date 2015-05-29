#include "MddProxy.hxx"
#include <boost/foreach.hpp>
#include "ReceiverSocket.hxx"
#include "SenderSocket.hxx"
#include "MddProxyException.hxx"
#include "Socket.hxx"
#include "MddProxyRunLogger.hxx"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

namespace mdm {
namespace mddproxy {

Logger* LoggerInstance;
void CreateLoggerInstance(std::string& logFileDir, std::string& fileName)
{
	LoggerInstance = new Logger(logFileDir, fileName);
}

MddProxy::MddProxy(const std::string& configFileName, const ConfigFileFormatT& configFormat, std::string& logFileDir, std::string& logLevel)
{
	configReader.Load(configFileName,configFormat);
	std::string fileName = "/MddProxy.log";
	CreateLoggerInstance(logFileDir, fileName);
	receivers = new WorkItemsT;
	senders = new WorkItemsT;
	AddReceiversFromConfig(configReader.GetMappedFeeds());
	workDispatcher = new WorkDispatcher(receivers);
	continueProcessing = true;
}

MddProxy::~MddProxy()
{
	delete workDispatcher;

	if (receivers)
	{
		WorkItemsItT first = receivers->begin();
		WorkItemsItT last = receivers->end();

		for(;first!=last; first++)
		{
			Socket* socket= first->second;
			if (socket)
				delete socket;
		}
		receivers->clear();
	}
	delete receivers;
	delete senders;

}

void MddProxy::FillAddr(AddrT* addr,MulticastGroupT& multicastGroup)
{
	inet_aton(multicastGroup.address.c_str(),&addr->addr.sin_addr);
	addr->addr.sin_port = multicastGroup.port;
	switch (multicastGroup.protocol)
	{
	case UDP:
		addr->protocol = SOCK_DGRAM;
		break;
	case TCP:
		addr->protocol = SOCK_STREAM;
		break;
	}
	addr->type = multicastGroup.broadcastType;

	addr->addr.sin_family =  AF_INET;
}

void MddProxy::AddReceiversFromConfig(MappedFeedListT* feedsList)
{
	BOOST_FOREACH(MappedFeedListT::value_type item,*feedsList)
	{
		InterfaceConfigT* mappedFeed = item;
		BOOST_FOREACH(ReceiverSendersT* rcvrSenders,mappedFeed->rcvrSenders)
		{
			SenderListT* senderList = new SenderListT;
			BOOST_FOREACH(OutputAddressT* output, rcvrSenders->outputAddresses)
			{
				AddrT* senderAddr = new AddrT;
				senderAddr->interface.sin_addr.s_addr = inet_addr(mappedFeed->outputInterface.c_str());
				THROW_IF(senderAddr->interface.sin_addr.s_addr == INADDR_NONE,
						"Invalid output interface address <%s>", (const char*) mappedFeed->outputInterface.c_str());
				this->FillAddr(senderAddr, *output);

				mdm::mddproxy::Socket* senderObj = new SenderSocket(senderAddr);
				senderObj->Create();
				senderList->push_back(senderObj);
			}

			AddrT* receiverAddr = new AddrT;

			if (mappedFeed->inputInterface == "0.0.0.0")
				receiverAddr->interface.sin_addr.s_addr = htonl(INADDR_ANY);
			else
				receiverAddr->interface.sin_addr.s_addr = inet_addr(mappedFeed->inputInterface.c_str());

			THROW_IF(receiverAddr->interface.sin_addr.s_addr == INADDR_NONE,
					"Invalid input interface address <%s>", (const char*) mappedFeed->inputInterface.c_str());

			this->FillAddr(receiverAddr, *rcvrSenders->inputAddress);
			receiverAddr->interface.sin_port = receiverAddr->addr.sin_port;
			receiverAddr->interface.sin_family = receiverAddr->addr.sin_family;

			ReceiverSocket* rcvrSocket = new ReceiverSocket(senderList, receiverAddr);
			receiverAddr = rcvrSocket->Create();
			this->receivers->insert(std::make_pair(receiverAddr->socket,rcvrSocket));

			epoller.RegisterSocket(receiverAddr->socket);
		}
	}
}

}
}
