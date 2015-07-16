/*
 * MarketDataFeedConfigReader.hxx
 *
 *  Created on: May 5, 2015
 *      Author: kheddar
 */

#ifndef MARKETDATAFEEDCONFIGREADER_HXX_
#define MARKETDATAFEEDCONFIGREADER_HXX_

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/unordered_map.hpp>
#include <iostream>
#include <vector>
#include <string>

#include "CommonDefinitions.hxx"

namespace pt = boost::property_tree;

namespace mdm {
namespace mddproxy {

typedef enum
{
	UDP = 0,
	TCP = 1
}ProtocolT;

typedef struct
{
	uint32_t id;
	std::string address;
	uint32_t port;
	BroadcastT broadcastType;
	ProtocolT protocol;
} MulticastGroupT;

typedef struct : MulticastGroupT
{
	std::string feedName;
} InputAddressT;


typedef struct : MulticastGroupT
{
	bool isLoopBack;
}OutputAddressT;

typedef struct
{
	InputAddressT* inputAddress;
	std::list<OutputAddressT*> outputAddresses;
}ReceiverSendersT;

typedef struct
{
	std::string inputInterface;
	std::string outputInterface;
	std::list<ReceiverSendersT*> rcvrSenders;
}InterfaceConfigT;

typedef enum
{
	XML = 1,
	INI = 2,
	INFO = 3,
	JSON = 4
} ConfigFileFormatT;

typedef std::list<InterfaceConfigT*> MappedFeedListT;

using namespace pt;
class MddFeedConfig
{

	const ptree& empty_ptree()
	{
		static ptree t;
		return t;
	}

public:
	MddFeedConfig();
	~MddFeedConfig();

	void Load(const std::string& filename, const ConfigFileFormatT& format);
	MappedFeedListT* GetMappedFeeds();

private:
	void LoadReceivers(ptree& tree);
	void LoadSenders(ptree& tree);
	void LoadMappedFeeds(ptree& tree);

	boost::unordered_map<uint32_t, InputAddressT*> receivers;
	boost::unordered_map<uint32_t, OutputAddressT*> senders;

	MappedFeedListT* mappedFeeds;
};


} /* namespace mddproxy */
} /* namespace mdm */

#endif /* MARKETDATAFEEDCONFIGREADER_HXX_ */
