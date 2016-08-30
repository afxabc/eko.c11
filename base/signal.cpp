#include "signal.h"
#include "timestamp.h"


using namespace eko;

////////////////////////////////////////////////////

Signal::Signal(bool on, bool auto_off) : auto_(auto_off), signal_(on)
{
#ifdef WIN32
	handle_ =  CreateEvent(NULL, auto_?FALSE:TRUE, signal_?TRUE:FALSE, NULL );
#else
	int  rc =  pthread_cond_init(&handle_,0);
	assert( rc == 0 );
    rc = pthread_mutex_init(&mutex_, NULL);
    assert( rc == 0 );
#endif
}

Signal::~Signal()
{
#ifdef WIN32
	CloseHandle(handle_);
#else
	if (pthread_cond_destroy(&handle_) == EBUSY)
		assert(0);
	int rc = pthread_mutex_destroy(&mutex_);
    assert( rc == 0 );
#endif
}

void Signal::wait()
{
#ifdef WIN32
   WaitForSingleObject(handle_, INFINITE);
#else
    int  rc = pthread_mutex_lock(&mutex_);
    if ( rc != 0 )
		return;
	rc = 0;
	while (!signal_ && rc == 0)
		rc = pthread_cond_wait(&handle_, &mutex_);
	if (rc == 0 && auto_)
		signal_ = false;
	pthread_mutex_unlock(&mutex_);
#endif
}

bool Signal::wait(MicroSecond delay)
{
	if (delay < 0)
		delay = 0;

#ifdef WIN32
	DWORD rc = WaitForSingleObject(handle_, (int)delay);
	return (rc == WAIT_OBJECT_0);

#else
    int  rc = pthread_mutex_lock(&mutex_);
    if ( rc != 0 )
		return false;

	if (delay > 0)
	{
	/*
		Timestamp exp = Timestamp::NOW()+delay;
		UInt64 time = exp;

		timespec tm;
		tm.tv_sec = time / 1000;
		tm.tv_nsec = (time % 1000) * 1000000L;
		assert( tm.tv_nsec < 1000000000L );
		*/

		timespec tm;
        clock_gettime(CLOCK_REALTIME, &tm);
		tm.tv_sec += delay / 1000;
		tm.tv_nsec += (delay % 1000) * 1000000L;

		rc = 0;
		while (!signal_ && rc == 0)
			rc = pthread_cond_timedwait(&handle_, &mutex_, &tm);
	}
	else rc = (signal_)?0:ETIMEDOUT;	//delay == 0

	if (rc == 0 && auto_)
		signal_ = false;

	pthread_mutex_unlock(&mutex_);
	return (!(rc == EINTR || rc == ETIMEDOUT));
#endif
}

void Signal::on()
{
#ifdef WIN32
   SetEvent(handle_);
#else
    int  rc = pthread_mutex_lock(&mutex_);
    if ( rc != 0 )
		return;

	if (!signal_) {
		signal_ = true;
		pthread_cond_signal(&handle_);
	}

	pthread_mutex_unlock(&mutex_);
#endif
}

void Signal::off()
{
#ifdef WIN32
   ResetEvent(handle_);
#else
    int  rc = pthread_mutex_lock(&mutex_);
    if ( rc != 0 )
		return;

	signal_ = false;

	pthread_mutex_unlock(&mutex_);
#endif
}

