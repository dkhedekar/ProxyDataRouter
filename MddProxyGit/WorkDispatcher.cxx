/*
 * WorkDispatcher.cxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#include "WorkDispatcher.hxx"
#include "MddProxyException.hxx"
#include "MddProxyRunLogger.hxx"
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
#include <boost/foreach.hpp>

namespace mdm {
namespace mddproxy {

WorkDispatcher::WorkDispatcher(WorkItemsT* rcvrs, size_t maxWorkerThreads)
{
	receivers = rcvrs;
	pendingItems = new PendingWorkQueue<Socket*>();
	freeWorkers = new std::deque<Worker*>;
	busyWorkers = new std::deque<Worker*>;

	totalWorkerCount = 0;
	AddWorkers(5);
}

WorkDispatcher::~WorkDispatcher() {

	pendingItems->Clear();
	delete pendingItems;

}

int WorkDispatcher::AddWorkers(size_t count)
{
	for (size_t i=0; i < count; i++)
	{
		Worker* newWorker = CreateNewWorker();
		freeWorkers->push_back(newWorker);
	}
	LOGINF("Added %d new workers. Total worker count %d", count, totalWorkerCount);
	return count;
}

Worker* WorkDispatcher::CreateNewWorker()
{
	Worker* newWorker = new Worker(totalWorkerCount, this);
	pthread_t* thId = newWorker->Create();

	LOGINF("Created new worker id %ld", thId);
	workers.push_back(newWorker);

	totalWorkerCount++;

	return newWorker;
}

void WorkDispatcher::WakeUpAllThreads()
{
	boost::mutex::scoped_lock lock(sync_lock);

	BOOST_FOREACH(Worker *worker, workers)
	{
		worker->FinishAndDie();
	}
}

void WorkDispatcher::ProcessData(struct epoll_event* evList, int eventCount)
{
	for (int i=0; i < eventCount; i++)
	{
		WorkItemsItT result = receivers->find(evList[i].data.fd);

		THROW_IF(result == receivers->end(), "receiver not found fd=>%d",evList[i].data.fd );

		Worker*  worker;
		if (GetFreeWorker(worker))
		{
			//result->second->ReceiveData();
			worker->GetParams()->receiverSocket = result->second;
			worker->Resume();
		}
		else if (totalWorkerCount < MAX_WORKER_THREADS)
		{
			worker = CreateNewWorker();
			// allocate worker
			//result->second->ReceiveData();
			worker->GetParams()->receiverSocket = result->second;
			worker->Resume();
			RecordNewBusyWorker(worker);

			worker->Create();
		}
		else
		{
			pendingItems->Push(result->second);
		}
	}
}

bool WorkDispatcher::GetFreeWorker(Worker* &worker)
{
	boost::mutex::scoped_lock lock(sync_lock);

	if (freeWorkers->size() > 0)
	{
		worker = freeWorkers->front();
		freeWorkers->pop_front();
		busyWorkers->push_back(worker);
		return true;
	}

	return false;
}

void WorkDispatcher::RecordNewBusyWorker(Worker* worker)
{
	boost::mutex::scoped_lock lock(sync_lock);

	busyWorkers->push_back(worker);
}

size_t WorkDispatcher::ReturnFreeWorker(Worker* worker)
{
	boost::mutex::scoped_lock lock(sync_lock);

	freeWorkers->push_back(worker);
	return freeWorkers->size();
}

void WorkDispatcher::JoinThreads()
{
	BOOST_FOREACH(Worker *worker, workers)
	{
		pthread_join(*worker->GetThreadId(), NULL);
		delete worker;
	}
}

} /* namespace mddproxy */
} /* namespace mdm */
