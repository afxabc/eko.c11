#pragma once
#include "common.h"
#include "queue.h"
#include <thread>
#include <vector>

namespace eko
{
	class ThreadPool : public Noncopyable
	{
	public:
		ThreadPool();
		~ThreadPool();

		void start(int numThreads = 4);
		void stop();
		bool run(Functor&& func);

		int pending()
		{
			return (int)queue_.size();
		}

	private:
		void runInThread();
		bool take(Functor& func);

	private:
		Queue<Functor> queue_;
		Mutex mutex_;
		std::vector<std::thread> threads_;
		bool running_;
	};

}
