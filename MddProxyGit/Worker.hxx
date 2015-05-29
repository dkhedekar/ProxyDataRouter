/*
 * Worker.hxx
 *
 *  Created on: May 12, 2015
 *      Author: kheddar
 */

#ifndef WORKER_HXX_
#define WORKER_HXX_

#include <pthread.h>
#include <stdint.h>
#include "Socket.hxx"
#include "MddProxyException.hxx"
#include "CommonDefinitions.hxx"

namespace mdm {
namespace mddproxy {

typedef enum
{
	SLEEP_WAIT,
	SLEEP_TIME,
	RESUME,
	TERMINATE
} WorkerStateT;

typedef struct
{
	WorkerStateT state;
	uint32_t sleepTime;
	Socket* receiverSocket;
} WorkerStateParamsT;

class WorkDispatcher;

class Worker
{
public:
	explicit Worker(size_t slotId, WorkDispatcher* dispatcher);
	virtual ~Worker();

	void Spin(uint32_t milliseconds); // Not implemented.
	void Pause(uint32_t milliseconds = 0);
	void Resume();
	void FinishAndDie();

	WorkerStateParamsT* GetParams();

	pthread_t* Create()
	{
		thread = new pthread_t;
		int retCode = pthread_create(thread, NULL, &Worker::RunHelper, (void*)this);
		if (retCode != 0 )
			THROW("Can't create thread");

		return thread;
	}

	static void *RunHelper(void *context)
	{
		return ((Worker *)context)->Run();
	}

	void* Run();

	pthread_t* GetThreadId()
	{
		return thread;
	}

private:
	pthread_t* thread;
	size_t mySlotId;
	WorkerStateParamsT* myControlSlot;
	WorkDispatcher* myDispatcher;

	pthread_cond_t thread_wait_cond;
	pthread_mutex_t sync_lock;

	char errBuffer[ERROR_BUFF_SIZE];

};

} /* namespace mddproxy */
} /* namespace mdm */

#endif /* WORKER_HXX_ */
