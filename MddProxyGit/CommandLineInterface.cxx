/*
 * CommandLineInterface.cxx
 *
 *  Created on: May 29, 2015
 *      Author: kheddar
 */

#include "CommandLineInterface.hxx"
#include "MddProxyException.hxx"
#include "MddProxyRunLogger.hxx"

#include <ifaddrs.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <getopt.h>
#include <exception>
#include <iostream>

#include <boost/foreach.hpp>


namespace mdm {
namespace mddproxy {

CommandLineInterface::CommandLineInterface(AddrT* addr, MddProxy* mddproxy):
		Socket(addr), proxy(mddproxy), clientSocket(0), stopCommandIntf(false)
{

}

CommandLineInterface::~CommandLineInterface()
{}

AddrT* CommandLineInterface::Create()
{
	socketObj->socket = socket( AF_INET, SOCK_STREAM, 0);

	THROW_IF( socketObj->socket < 0,
			"Error in can't create socket (inet addr:%d>) ERR: %s",
			(unsigned) ntohs(socketObj->addr.sin_port),
			strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));


	int option=1;
	int retCode = setsockopt(socketObj->socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&option, sizeof(option));

	THROW_IF( retCode < 0, "Error setting socket option");

	struct sockaddr_in localSock;
	memset((char *) &localSock, 0, sizeof(localSock));
	localSock.sin_family = AF_INET;
	localSock.sin_port = htons(socketObj->interface.sin_port);
	localSock.sin_addr.s_addr = INADDR_ANY;
	retCode = bind (socketObj->socket, (struct sockaddr *)&localSock, sizeof(localSock));

	THROW_IF(retCode < 0, "Can't bind to interface <%d> err: %s",
			(unsigned) ntohs(socketObj->addr.sin_port),
			strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	socklen_t optlen = sizeof(size_t);
	getsockopt(socketObj->socket, SOL_SOCKET, SO_RCVBUF, &socketBufferSize, &optlen);

	return socketObj;
}

void CommandLineInterface::Listen()
{
	int retCode = listen(socketObj->socket, 0);

	THROW_IF(retCode < 0, "Can't listen on interface %d> err: %s",
			(unsigned) ntohs(socketObj->addr.sin_port),
			strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	do
	{
		struct sockaddr clientAddr;
		socklen_t clientAddLen = sizeof (clientAddr);
		clientSocket = accept(socketObj->socket, &clientAddr , &clientAddLen);

		THROW_IF(clientSocket < 0, "Error accepting client connection on %d err: %s",
					(unsigned) ntohs(socketObj->addr.sin_port),
					strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

		ReceiveData();
	} while(!stopCommandIntf);

	if (stopCommandIntf)
		close(socketObj->socket);
}
void CommandLineInterface::ReceiveData()
{
	char* buff = new char[1024];

	while(!stopCommandIntf)
	{
		try
		{
			int readcount = read(clientSocket, buff, sizeof(buff));
			std::string response =	ProcessCommand(buff, readcount);
			SendData(response.c_str(),response.length());
		}
		catch(std::exception& ex)
		{
			LOGINF("Exception caught in processing command %s", ex.what());
			break;
		}
	}
}

void CommandLineInterface::SendData(const char* buff,size_t size)
{
	write(clientSocket,(char*)buff,size);
}
std::string CommandLineInterface::ProcessCommand(char* buffer, size_t inpBufferLen)
{
	pt::ptree tree;

    membuf stbuf(buffer, buffer + inpBufferLen);
    std::istream is(&stbuf);

    int retCode = 0 ;
	try
	{
    	pt::read_json(is, tree);

    	BOOST_FOREACH( ptree::value_type const&command, tree.get_child("commands") )
		{
    		std::string commandName = command.second.get<std::string>("<xmlattr>.name");
    		uint8_t id = command.second.get<uint8_t>("<xmlattr>.id");

    		switch(id)
    		{
    			case 1: // stopproc
					proxy->Stop();
					stopCommandIntf = true;
					break;
				case 2: // startstats
				{
					boost::property_tree::ptree commandPivot = command.second;
					boost::optional<uint32_t> optionalValue =  command.second.get_optional<uint32_t>("value");

					if (optionalValue.is_initialized() )
					{
						proxy->StartStats(optionalValue.get());
					}
					else
						proxy->StartStats();
				}
					break;
				case 3: //stopstats
				{
					proxy->StopStats();
				}
					break;
				case 4: //loglevel
				{
					boost::property_tree::ptree commandPivot = command.second;
					boost::optional<std::string> optionalValue =  command.second.get_optional<std::string>("value");


					LogLevelT newLogLevel = INFORMATIONAL;
					if (strcmp(optionalValue.get().c_str(), "DEBUG")==0 )
					{
						newLogLevel = DEBUG;
					}
					proxy->SetLogLevel(newLogLevel);
				}
					break;

				case 5: // sndBufferSize
				{
					boost::property_tree::ptree commandPivot = command.second;
					boost::optional<uint32_t> optionalValue =  command.second.get_optional<uint32_t>("value");

					proxy->SetSendBufferSize(optionalValue.get());
					break;
				}
				case 6: // rcvBufferSize
				{
					boost::property_tree::ptree commandPivot = command.second;
					boost::optional<uint32_t> optionalValue =  command.second.get_optional<uint32_t>("value");
					proxy->SetRecvBufferSize(optionalValue.get());
					break;
				}

				default:
					retCode = -1;
					break;
			}
		}
	}
	catch(std::exception& ex)
	{
		retCode = -1;
		LOGINF("Exception caught in processing command %s", ex.what());
	}

	// creating response

	boost::property_tree::ptree retRoot, response;
	response.put<std::string>("retCode", (retCode == 0)? "success": "error");
	response.put<std::string>("msg", "");
	retRoot.put_child("path1.path2", response);

	std::stringstream ss;
	write_json(ss, retRoot);

	return ss.str();
}

} /* namespace mddproxy */
} /* namespace mdm */
