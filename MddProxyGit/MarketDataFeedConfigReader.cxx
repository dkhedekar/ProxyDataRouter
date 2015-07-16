/*
 * MarketDataFeedConfigReader.cxx
 *
 *  Created on: May 5, 2015
 *      Author: kheddar
 */

#include "MarketDataFeedConfigReader.hxx"
#include <exception>
#include <boost/foreach.hpp>
#include "MddProxyException.hxx"

namespace mdm {
namespace mddproxy {

MddFeedConfig::MddFeedConfig()
{
	mappedFeeds = new MappedFeedListT;
}

MddFeedConfig::~MddFeedConfig()
{
	BOOST_FOREACH( MappedFeedListT::value_type item, *mappedFeeds)
	{
		delete item;
	}

	delete mappedFeeds;
}
//]
//[debug_settings_load
void MddFeedConfig::Load(const std::string& filename, const ConfigFileFormatT& format)
{
    // Create empty property tree object
    pt::ptree tree;

    switch (format)
    {
    	case XML:
    		pt::read_xml(filename, tree);
    		break;
    	default:
    		THROW("Unrecognizable Mdd feed Config file format. Allowed formats are xml/ini/info/json");
    		break;
    }

    LoadReceivers(tree);
    LoadSenders(tree);
    LoadMappedFeeds(tree);

}


void MddFeedConfig::LoadReceivers(ptree& tree)
{
	BOOST_FOREACH( ptree::value_type const&receiver, tree.get_child("Receivers") )
	{
		if( receiver.first == "Receiver" )
		{
			InputAddressT* inputAddr = new InputAddressT;
			inputAddr->feedName = receiver.second.get<std::string>("<xmlattr>.name");
			inputAddr->id = receiver.second.get<uint32_t>("<xmlattr>.id");
			inputAddr->address = receiver.second.get<std::string>("<xmlattr>.ip");
			inputAddr->port = receiver.second.get<uint32_t>("<xmlattr>.port");

			std::string bcastType = receiver.second.get<std::string>("<xmlattr>.type");

			if ( bcastType == "Unicast")
				inputAddr->broadcastType = Unicast;
			else
				inputAddr->broadcastType = Multicast;

			std::string protocol = receiver.second.get<std::string>("<xmlattr>.protocol");
			if ( protocol == "UDP")
				inputAddr->protocol = UDP;
			else
				inputAddr->protocol = TCP;


			receivers.insert(std::make_pair(inputAddr->id,inputAddr));
		}
	}
}
void MddFeedConfig::LoadSenders(ptree& tree)
{
	BOOST_FOREACH( ptree::value_type const&sender, tree.get_child("Senders") )
	{
		if( sender.first == "Sender" )
		{
			OutputAddressT* outputAddr = new OutputAddressT;
			outputAddr->id = sender.second.get<uint32_t>("<xmlattr>.id");
			outputAddr->address = sender.second.get<std::string>("<xmlattr>.ip");
			outputAddr->port = sender.second.get<uint32_t>("<xmlattr>.port");

			outputAddr->isLoopBack = sender.second.get("<xmlattr>.isLoopback", false);
			std::string bcastType = sender.second.get<std::string>("<xmlattr>.type");
			if ( bcastType == "Unicast")
				outputAddr->broadcastType = Unicast;
			else
				outputAddr->broadcastType = Multicast;

			std::string protocol = sender.second.get<std::string>("<xmlattr>.protocol");
			if ( protocol == "UDP")
				outputAddr->protocol = UDP;
			else
				outputAddr->protocol = TCP;

			senders.insert(std::make_pair(outputAddr->id, outputAddr));
		}
	}
}
void MddFeedConfig::LoadMappedFeeds(ptree& tree)
{
	BOOST_FOREACH( ptree::value_type const&map, tree.get_child("Mapping") )
	{
		if( map.first == "Map" )
		{
			InterfaceConfigT* interfaceMap = new InterfaceConfigT;
			interfaceMap->inputInterface = map.second.get<std::string>("<xmlattr>.inputInterface");
			interfaceMap->outputInterface = map.second.get<std::string>("<xmlattr>.outputInterface");

			boost::property_tree::ptree subtree = map.second;

			BOOST_FOREACH( ptree::value_type const&rcvr, subtree.get_child("") )
			{
				if( rcvr.first == "MappedReceiver" )
				{
					ReceiverSendersT* rcvrSenders = new ReceiverSendersT;
					uint32_t receiverId = rcvr.second.get<uint32_t>("<xmlattr>.id");

					boost::unordered_map<uint32_t, InputAddressT*>::iterator inputAddrIt = receivers.find(receiverId);

					if (inputAddrIt != receivers.end())
					{
						InputAddressT* inputAddr = inputAddrIt->second;
						rcvrSenders->inputAddress = inputAddr;
					}
					boost::property_tree::ptree subtreesenders = rcvr.second;

					BOOST_FOREACH(ptree::value_type const&sender, subtreesenders.get_child("") )
					{
						if( sender.first == "MappedSender" )
						{
							uint32_t senderId = sender.second.get<uint32_t>("<xmlattr>.id");
							boost::unordered_map<uint32_t, OutputAddressT*>::iterator outputAddrIt = senders.find(senderId);
							if (outputAddrIt != senders.end())
							{
								OutputAddressT* outputAddr = outputAddrIt->second;
								rcvrSenders->outputAddresses.push_back(outputAddr);
							}
						}
					}
					interfaceMap->rcvrSenders.push_back(rcvrSenders);
				}
			}

			mappedFeeds->push_back(interfaceMap);
		}
	}
}

MappedFeedListT* MddFeedConfig::GetMappedFeeds()
{
	return mappedFeeds;
}
} /* namespace mddproxy */
} /* namespace mdm */
