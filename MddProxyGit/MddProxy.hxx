/*
 * MddProxy.hxx
 *
 *  Created on: May 8, 2015
 *      Author: kheddar
 */

#ifndef MDDPROXY_HXX_
#define MDDPROXY_HXX_


#include <string>
#include "MarketDataFeedConfigReader.hxx"
#include "WorkDispatcher.hxx"
#include "Epoller.hxx"
#include "MddProxyRunLogger.hxx"

namespace mdm {
namespace mddproxy {

class MddProxy {
public:
	MddProxy(const std::string& configFileName, const ConfigFileFormatT& configFormat, std::string& logFileDir, std::string& logLevel);
	virtual ~MddProxy();

	void WaitForExit();
	void SetLogLevel(LogLevelT);

	void Stop()
	{
		continueProcessing = false;
	}
private:
	MddFeedConfig configReader;
	WorkDispatcher* workDispatcher;
	mdm::mddproxy::WorkItemsT* receivers;
	mdm::mddproxy::WorkItemsT* senders;
	bool continueProcessing;

	void AddReceiversFromConfig(MappedFeedListT* feedsList);
	void FillAddr(AddrT* addr,MulticastGroupT& multicastGroup);

	Epoller epoller;

};

inline void MddProxy::WaitForExit()
{
	do
	{
		epoller.WaitForData(workDispatcher);
	}while(continueProcessing);

	workDispatcher->JoinThreads();
}
} /* namespace mddproxy */
} /* namespace mdm */

#endif /* MDDPROXY_HXX_ */
