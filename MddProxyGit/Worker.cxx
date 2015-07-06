/*
 * Worker.cxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#include "Worker.hxx"
#include "WorkDispatcher.hxx"
#include "MddProxyRunLogger.hxx"

namespace mdm {
namespace mddproxy {

Worker::Worker(size_t slotId, WorkDispatcher* dispatcher):
		thread(NULL),
		mySlotId(slotId),myControlSlot(new WorkerStateParamsT),
		myDispatcher(dispatcher),
		thread_wait_cond(PTHREAD_COND_INITIALIZER),
		sync_lock(PTHREAD_MUTEX_INITIALIZER)
{
	myControlSlot->state = SLEEP_WAIT;
}

Worker::~Worker()
{
	LOGINF("Deleting worker %ld", thread);
	delete myControlSlot;
}

WorkerStateParamsT* Worker::GetParams()
{
	return myControlSlot;
}

void Worker::Spin(uint32_t milliseconds)
{}

void Worker::Pause(uint32_t milliseconds)
{
	int retCode = pthread_mutex_lock(&sync_lock);
	THROW_IF( retCode!=0, "Can't lock mutex in thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	while (myControlSlot->state == SLEEP_WAIT || myControlSlot->state == SLEEP_TIME)
	{
		if ( milliseconds == 0)
		{
			retCode = pthread_cond_wait(&this->thread_wait_cond,&sync_lock);
			THROW_IF( retCode!=0, "Can't wait thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));
		}
		else
		{
			struct timespec sleepTime;
			sleepTime.tv_nsec = milliseconds*1000;
			sleepTime.tv_sec = 0;
			retCode = pthread_cond_timedwait(&this->thread_wait_cond,&sync_lock,&sleepTime);
			THROW_IF( retCode!=0, "Can't wait thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));
		}
	}

	retCode = pthread_mutex_unlock(&sync_lock);
	THROW_IF( retCode!=0, "Can't unlock mutex in thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));
}

void Worker::Resume()
{
	int retCode = pthread_mutex_lock(&sync_lock);
	THROW_IF( retCode!=0, "Can't lock mutex in thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	myControlSlot->state = RESUME;

	retCode = pthread_cond_signal(&thread_wait_cond);
	THROW_IF( retCode!=0, "Can't signal thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

	retCode = pthread_mutex_unlock(&sync_lock);
	THROW_IF( retCode!=0, "Can't unlock mutex in thread %lu %s", *thread, strerror_r(errno, errBuffer, ERROR_BUFF_SIZE));

}

void Worker::FinishAndDie()
{
	myControlSlot->state = TERMINATE;
	pthread_cond_signal(&thread_wait_cond);
	pthread_exit(&thread);
}

void* Worker::Run()
{
	try
	{
		while(true)
		{
			if (myControlSlot->state == RESUME)
			{
				myControlSlot->receiverSocket->ReceiveData();
				myDispatcher->OnWorkComplete(this);
			}

			else if (myControlSlot->state == SLEEP_WAIT)
				Pause();

			else if (myControlSlot->state == SLEEP_TIME)
				Pause(myControlSlot->sleepTime);

			else if (myControlSlot->state == TERMINATE)
				break;

		}
	}
	catch(std::exception& ex)
	{
		THROW(ex.what());
	}

	return (void*)NULL;
}


} /* namespace mddproxy */
} /* namespace mdm */
