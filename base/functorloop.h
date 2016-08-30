#ifndef _BASE_FUNCTOR_LOOP_H__
#define _BASE_FUNCTOR_LOOP_H__ 

#include "thread.h"
#include "signal.h"
#include "functorqueue.h"


namespace eko
{
	typedef std::function<void(MicroSecond)> WaitFunctor;
	class FunctorLoop : public Noncopyable
	{
	public:
		FunctorLoop();
		~FunctorLoop();

		bool isRun()
		{
			return run_;
		}

		void loop(); //进入loop

		void quitLoop();		//退出loop

		bool loopInThread(Thread::ThreadPriority pri = Thread::THREAD_PRI_NONE)	//在新的线程loop
		{
			return thread_.start(std::bind(&FunctorLoop::loop, this), pri);
		}

		bool isInLoopThread()
		{
			return (threadId_ == Thread::self());
		}

		UInt32 runInLoop(Functor&& func, MicroSecond delay = 0);

		bool cancel(UInt32 sequence)
		{
			return queue_.cancel(sequence);
		}

		void setLoop(Functor fun)
		{
			loopFun_ = fun;
		}

		void setWait(WaitFunctor fun)
		{
			waitFun_ = fun;
		}

		void setWakeup(Functor fun)
		{
			wakeupFun_ = fun;
		}

		MicroSecond runQueue()		//运行到时的函数
		{
			return queue_.run();
		}

		int getPending()
		{
			return (int)queue_.size();
		}

		int getActPending()
		{
			return (int)queue_.actSize();
		}

	private:
		void defaultLoop();
		void defaultWait(MicroSecond ms);
		void defaultWakeup();

	protected:
		Functor loopFun_;
		WaitFunctor waitFun_;
		Functor wakeupFun_;

		FunctorQueue queue_;
		Thread thread_;
		UInt32 threadId_;
		bool run_;
		Signal signal_;
	};
}
#endif
