/*
 * WorkDispatcher.hxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#ifndef WORKDISPATCHER_HXX_
#define WORKDISPATCHER_HXX_

#include "PendingWorkQueue.hxx"
#include "Worker.hxx"
#include "Socket.hxx"

#include <vector>
#include <map>
#include <sys/epoll.h>

#define MAX_WORKER_THREADS 10

namespace mdm {
namespace mddproxy {

/**
 * @brief
 * Dispatches work from the work queue to the worker threads
 * Spawns more worker threads if needed
 */
typedef std::map<int,Socket*> WorkItemsT;
typedef WorkItemsT::iterator WorkItemsItT;

class WorkDispatcher {
public:
	explicit WorkDispatcher(WorkItemsT* rcvrs, size_t maxWorkerThreads = MAX_WORKER_THREADS);
	virtual ~WorkDispatcher();

	void ProcessData(struct epoll_event* evList, int eventCount);
	void OnWorkComplete(Worker* worker);

	void JoinThreads();

	int AddWorkers(size_t count=1);

	void WakeUpAllThreads();

private:
	WorkItemsT*  receivers;
	PendingWorkQueue<Socket*>* pendingItems;
	std::deque<Worker*>* freeWorkers;
	std::deque<Worker*>* busyWorkers;

	std::vector<Worker*> workers;
	size_t totalWorkerCount;

	boost::mutex sync_lock;

	Worker* CreateNewWorker();

	bool GetFreeWorker(Worker* &worker);
	void RecordNewBusyWorker(Worker* worker);
	size_t ReturnFreeWorker(Worker* worker);
};

inline void WorkDispatcher::OnWorkComplete(Worker* worker)
{

	if (pendingItems->Count() > 0)
	{
		worker->GetParams()->state = RESUME;
		worker->GetParams()->receiverSocket = pendingItems->Pop();
	}
	else
	{
		worker->GetParams()->state = SLEEP_WAIT;
		ReturnFreeWorker(worker);
	}
}
} /* namespace mddproxy */
} /* namespace mdm */

#endif /* WORKDISPATCHER_HXX_ */
