/*
 * Epoller.cxx
 *
 *  Created on: May 19, 2015
 *      Author: kheddar
 */

#include "Epoller.hxx"
#include "WorkDispatcher.hxx"

namespace mdm {
namespace mddproxy {

Epoller::Epoller()
{
	//ePollHandle = epoll_create1(0);
	ePollHandle = epoll_create1(EPOLL_CLOEXEC);
	fdCount = 0;

	events = new struct epoll_event[fdCount];
}

Epoller::~Epoller()
{
	delete events;
}

void Epoller::RegisterSocket(int sock)
{
	struct epoll_event event;
	event.data.fd = sock;
	event.events = EPOLLIN | EPOLLRDHUP | EPOLLET | EPOLLERR;

	char errBuffer[ERROR_BUFF_SIZE];

	int retCode = epoll_ctl(ePollHandle, EPOLL_CTL_ADD, sock, &event );

	THROW_IF( retCode < 0, "Cant't add socket handle to epoll %d %s", sock, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE))

	fdCount++;
}

void Epoller::WaitForData(WorkDispatcher* dispatcher)
{
	memset( events, 0, sizeof (struct epoll_event) * fdCount);
	int retFds = epoll_wait(ePollHandle, events, fdCount, -1 );

	if (retFds > 0)
	{
		dispatcher->ProcessData(events,retFds);
	}
}

void Epoller::CancelWait()
{
	close(ePollHandle);
}
} /* namespace mddproxy */
} /* namespace mdm */
