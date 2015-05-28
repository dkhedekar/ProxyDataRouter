/*
 * PendingWorkQueue.hxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#ifndef PENDINGWORKQUEUE_HXX_
#define PENDINGWORKQUEUE_HXX_

#include <deque>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>
namespace mdm {
namespace mddproxy {

//! @TODO Do we need priority concept?

template<typename T>
class PendingWorkQueue {
public:
	PendingWorkQueue();
	virtual ~PendingWorkQueue();

	size_t Push(T item);
	size_t PushUnsync(T item);

	size_t PushFront(T item);
	size_t PushFrontUnsync(T item);

	T Pop();
	T PopUnsync();

	size_t Count();
;
	void Clear();

private:
	std::deque<T>* pendingList;
	boost::mutex syncLock;
};

template <typename T>
PendingWorkQueue<T>::PendingWorkQueue()
{
	pendingList = new std::deque<T>;
}
template <typename T>
PendingWorkQueue<T>::~PendingWorkQueue()
{
	pendingList->clear();
	delete pendingList;
}

template <typename T>
size_t PendingWorkQueue<T>::PushUnsync(T item)
{
	pendingList->push_back(item);
	return pendingList->size();
}
template <typename T>
size_t PendingWorkQueue<T>::Push(T item)
{
	boost::mutex::scoped_lock lock(syncLock);
	return PushUnsync(item);
}

template <typename T>
size_t PendingWorkQueue<T>::PushFrontUnsync(T item)
{
	pendingList->push_front(item);
	return pendingList->size();
}
template <typename T>
size_t PendingWorkQueue<T>::PushFront(T item)
{
	boost::mutex::scoped_lock lock(syncLock);
	return PushFrontUnsync(item);
}

template <typename T>
T PendingWorkQueue<T>::Pop()
{
	boost::mutex::scoped_lock lock(syncLock);
	return PopUnsync();
}
template <typename T>
T  PendingWorkQueue<T>::PopUnsync()
{
	T value =  pendingList->front();
	pendingList->pop_front();

	return value;
}

template <typename T>
size_t  PendingWorkQueue<T>::Count()
{
	return pendingList->size();
}

template <typename T>
void  PendingWorkQueue<T>::Clear()
{
	return pendingList->clear();
}

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* PENDINGWORKQUEUE_HXX_ */
