
#include "common.h"
#include "thread.h"
#include "log.h"

#ifdef WIN32
#include <process.h>
#endif

using namespace eko;

#ifdef WIN32
const int Thread::osThreadPriority[THREAD_PRI_NONE] = {THREAD_PRIORITY_IDLE, THREAD_PRIORITY_LOWEST, THREAD_PRIORITY_NORMAL, THREAD_PRIORITY_HIGHEST, THREAD_PRIORITY_TIME_CRITICAL};
#else //1-99
const int Thread::osThreadPriority[THREAD_PRI_NONE] = {1, 15, 30, 45, 60};
#endif

#ifdef WIN32
UINT WINAPI Thread::_threadFunc(void* p)
#else
void* Thread::_threadFunc(void* p)
#endif
{
   Thread* pth = static_cast<Thread*>(p);

   assert(pth != NULL);

#ifdef WIN32
   if (pth->priority_ != THREAD_PRI_NONE)
    {
        ::SetThreadPriority(pth->handle_, osThreadPriority[pth->priority_]);
        LOGD("Thread priority=%d", GetThreadPriority(pth->handle_));
	}
#else

   if (pth->priority_ != THREAD_PRI_NONE)
    {
        int policy;
        struct sched_param param;
        pthread_getschedparam(pthread_self(), &policy, &param);
//        LOGD("Thread policy=%d priority=%d", policy, param.__sched_priority);
	}
#endif

   pth->func_();
   pth->stop();

#ifdef WIN32
   _endthreadex(0);
#endif

   return 0;
}

//////////////////////////////////////////////////////////////////////

Thread::Thread(void)
	: run_(false)
	, handle_(NULL)
	, thread_id_(0)
	, priority_(THREAD_PRI_NONE)
{
}

Thread::Thread(const Functor& func)
	: run_(false)
	, handle_(NULL)
	, thread_id_(0)
	, priority_(THREAD_PRI_NONE)
{
	start(func);
}

Thread::~Thread(void)
{
}

bool Thread::start(const Functor& func, ThreadPriority priority)
{
	if (run_)
		return false;

	func_ = func;
	run_ = true;
	priority_ = priority;

#ifdef WIN32
	handle_ = (HANDLE)_beginthreadex(NULL, 0, _threadFunc, this, 0, &thread_id_);
	run_ = (handle_ != NULL);
#else
	int retval = -1;
	if (priority_ != THREAD_PRI_NONE)
	{
        /*
        int rc = 0;
		struct sched_param param;
		param.__sched_priority = osThreadPriority[priority_];

        pthread_attr_init(&attr_);
		rc = pthread_attr_setinheritsched(&attr_, PTHREAD_EXPLICIT_SCHED);
		assert(rc == 0);
		rc = pthread_attr_setdetachstate(&attr_, PTHREAD_CREATE_DETACHED);
		assert(rc == 0);
		rc = pthread_attr_setschedpolicy(&attr_, SCHED_RR);
		assert(rc == 0);
		rc = pthread_attr_setschedparam(&attr_, &param);
		assert(rc == 0);

		retval = pthread_create(&thread_id_, &attr_, _threadFunc, (void*)this);
        pthread_attr_destroy(&attr_);
        */
        if (retval != 0)
        {
            priority_ = THREAD_PRI_NONE;
 //           LOGW("you have no right to change thread priority, back to normal.");
            retval = pthread_create(&thread_id_, NULL, _threadFunc, (void*)this);
        }
	}
	else retval = pthread_create(&thread_id_, 0, _threadFunc, (void*)this);
	assert(retval == 0);
	run_ = (retval == 0);
#endif

	return run_;
}

void Thread::stop()
{
	if (!run_)
		return;

	run_ = false;
#ifdef WIN32
	if (!isInThread())
	{
		WaitForSingleObject(handle_, INFINITE);
	}
	if (handle_ != NULL)
	{
		CloseHandle(handle_);
		handle_ = NULL;
	}
#else
	void* stat;
	if (!isInThread())
	{
		pthread_join( thread_id_ , &stat );
	}
#endif
	thread_id_ = 0;
}
