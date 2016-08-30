#ifndef BASE_THREAD_H_
#define BASE_THREAD_H_

#include "common.h"
#include "timestamp.h"

namespace eko
{
	class Thread : Noncopyable
	{
	public:
#ifdef WIN32
		typedef unsigned int THREAD_ID;
#else
		typedef int HANDLE;
		typedef pthread_t THREAD_ID;
#endif

		Thread(void);
		Thread(const Functor& func);
		~Thread(void);

		enum ThreadPriority
		{
			THREAD_PRI_IDLE,
			THREAD_PRI_LOW,
			THREAD_PRI_NORMAL,
			THREAD_PRI_HIGHT,
			THREAD_PRI_CRITICAL,
			THREAD_PRI_NONE,
		};

	public:
		inline static void sleep(MicroSecond ms)
		{
#ifdef WIN32
			Sleep(ms);
#else
			usleep(ms * 1000);
#endif
		}

		inline static THREAD_ID self()
		{
#ifdef WIN32
			return (THREAD_ID)GetCurrentThreadId();
#else
			return (THREAD_ID)pthread_self();
#endif
		}

	public:
		bool start(const Functor& func, ThreadPriority priority = THREAD_PRI_NONE);
		void stop();

		bool started() { return run_; }
		THREAD_ID id() { return thread_id_; }
		bool isInThread() { return (thread_id_ == self()); }

	private:
#ifdef WIN32
		static UINT WINAPI _threadFunc(void* p);
#else
		static void* _threadFunc(void* p);
#endif

	private:
		static const int osThreadPriority[THREAD_PRI_NONE];

	private:
		bool run_;
		HANDLE handle_;
		THREAD_ID thread_id_;

		ThreadPriority priority_;
		Functor func_;

#ifndef WIN32
		pthread_attr_t attr_;
#endif
	};
}

#endif
