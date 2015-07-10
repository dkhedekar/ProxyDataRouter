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
#include <string.h>
#include <iostream>
#include <getopt.h>
#include <exception>

static struct option commands[] = {
            {"sendbuff", required_argument, 0, 's'},
            {"recvbuff", required_argument, 0, 'r'},
            {"stop", no_argument, 0, 'e' },
            {"loglevel", required_argument,0,'l'},
            {"start-stats", optional_argument,0,'t'},
            {"stop-stats", no_argument,0,'n'}
        };


namespace mdm {
namespace mddproxy {

CommandLineInterface::CommandLineInterface(AddrT* addr, MddProxy* mddproxy):
		Socket(addr), proxy(mddproxy), clientSocket(0)
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
	} while(1);
}

void CommandLineInterface::ReceiveData()
{
	char* buff[1];
	buff[0] = new char[256];

	int retCode = read(clientSocket, buff[0], 256);

	int option_index = 0;
	bool shouldExit = false;

	try
	{
		while (1)
		{
			int c = getopt_long (1, buff, "dc:l",
					commands, &option_index);

			if (c == -1) break;

			switch (c)
			{
				case 's':
				{
					size_t buffersize = atoi(optarg);
					proxy->SetSendBufferSize(buffersize);
					break;
				}
				case 'r':
				{
					size_t buffersize = atoi(optarg);
					proxy->SetRecvBufferSize(buffersize);
					break;
				}
				case 'e':
					proxy->Stop();
					retCode = 0;
					shouldExit = true;
					break;
				case 'l':
				{
					LogLevelT newLogLevel = INFORMATIONAL;
					if (strcmp(optarg, "DEBUG")==0 )
					{
						newLogLevel = DEBUG;
					}
					proxy->SetLogLevel(newLogLevel);
				}
					break;
				case 't':
				{
					proxy->StartStats(atoi(optarg));
				}
					break;
				case 'n':
				{
					proxy->StopStats();
				}
					break;
				default:
					retCode = -1;
					break;
			}
		} // while
	}
	catch(std::exception& ex)
	{
		retCode = -1;
		LOGINF("Exception caught in processing command %s", ex.what());
	}
	memset(&buff[0], ' ', sizeof(buff[0]));
	strcpy(buff[0],(retCode == 0)? "success": "error");
	SendData(buff[0],strlen(buff[0]));

	if (shouldExit)
		close(socketObj->socket);
}

void CommandLineInterface::SendData(void* buff,size_t size)
{
	write(clientSocket,(char*)buff,size);
}
} /* namespace mddproxy */
} /* namespace mdm */
