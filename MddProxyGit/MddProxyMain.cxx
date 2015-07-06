#include <unistd.h>
#include <stdlib.h>
#include <iostream>
#include <getopt.h>
#include <signal.h>

#include "MarketDataFeedConfigReader.hxx"
#include "MddProxy.hxx"
#include "CommandLineInterface.hxx"

static struct option longOptions[] = {
            {"config", required_argument, 0, 'c'},
            {"logdir", required_argument, 0, 'd'},
            {"loglevel", required_argument, 0, 'l' },
            {"mgmtport", optional_argument, 0, 'm' }
        };

using namespace mdm::mddproxy;
MddProxy* mddPrxy;

void CheckForSignals(int signum)
{
	switch(signum)
	{
	case SIGINT:
	case SIGQUIT:
	case SIGHUP:
	case SIGABRT:
	case SIGBUS:
	case SIGPIPE:

		if (mddPrxy)
			mddPrxy->Stop();
		break;
	}
}

int main(int argc, char *argv[])
{
    int option_index = 0;
    std::string configFileName;
    std::string logFileDir = "/tmp/";
    std::string logLevel = "info";

    int mgmtport = 6800;

    while (1)
    {

    	int c = getopt_long (argc, argv, "dc:l",
                           longOptions, &option_index);

    	if (c == -1) break;

    	switch (c)
		{
			case 'c':
				configFileName = optarg;
				std::cout << "Reading Mdd config " << configFileName << std::endl;
				break;

			case 'd':
				logFileDir = optarg;
				std::cout << "Logging to file " << logFileDir << std::endl;
				break;

			case 'l':
				logLevel = optarg;
				std::cout << "Working with log level " << logLevel << std::endl;
				break;

			case 'm':
				mgmtport = atoi(optarg);
				std::cout << "Service mgmt through port " << mgmtport << std::endl;
				break;
			default: /* '?' */
				fprintf(stderr, "Usage: %s [-d <log file dir>] -c <config file name> " \
						"[-l <log level=debug|info>] \n",
						argv[0]);
				exit(EXIT_FAILURE);
		}
	} // while

    if (logLevel != "debug" && logLevel != "info" )
    {
    	fprintf(stderr, "Log Level should be debug|info and not %s", logLevel.c_str() );
    	exit(EXIT_FAILURE);
    }

    std::size_t found = configFileName.rfind('.');
    std::string configFileExtension = configFileName.substr(found,configFileName.size()-1);

    ConfigFileFormatT configFormat;
    if ( configFileExtension == ".xml" )
    	configFormat = XML;
    else if (configFileExtension == ".ini")
    	configFormat = INI;
    else if (configFileExtension == ".json")
        configFormat = JSON;
    else if (configFileExtension == ".info")
        configFormat = INFO;
    else 
    {
    	fprintf(stderr, "Unidentified mdd feed config format %s", configFileExtension.c_str() );
    	exit(EXIT_FAILURE);
    }

	signal(SIGINT, CheckForSignals);
	signal(SIGQUIT, CheckForSignals);
	signal(SIGABRT, CheckForSignals);

    mddPrxy = new MddProxy(configFileName, configFormat, logFileDir, logLevel);
    AddrT* mgmtAddr = new AddrT;
    mgmtAddr->interface.sin_port = mgmtport;
    CommandLineInterface* commandLineProc = new CommandLineInterface(mgmtAddr, mddPrxy);
    commandLineProc->Create();
    commandLineProc->Listen();

    mddPrxy->WaitForExit();
}
