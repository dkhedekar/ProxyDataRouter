/*
 * Epoller.hxx
 *
 *  Created on: May 19, 2015
 *      Author: kheddar
 */

#ifndef EPOLLER_HXX_
#define EPOLLER_HXX_

#include <sys/epoll.h>

namespace mdm {
namespace mddproxy {

class WorkDispatcher;

class Epoller {
public:
	Epoller();
	virtual ~Epoller();

	void RegisterSocket(int sock);
	void WaitForData(WorkDispatcher*);

	void CancelWait();

private:
	// Epoll handle
	int ePollHandle;
	uint32_t fdCount;
	struct epoll_event* events;
};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* EPOLLER_HXX_ */
