#include "threadpool.h"

eko::ThreadPool::ThreadPool()
	: running_(false)
	, queue_(100)
{
}

eko::ThreadPool::~ThreadPool()
{
	stop();
}

void eko::ThreadPool::start(int numThreads)
{
	if (running_ || numThreads <= 0)
		return;

	assert(threads_.empty());

	running_ = true;
	for (int i = 0; i < numThreads; ++i)
	{
		threads_.push_back(std::thread([this]() {this->runInThread(); }));
	}
}

void eko::ThreadPool::stop()
{
	running_ = false;
	queue_.signalAll();
	for (auto& th : threads_)
	{
		th.join();
	}
	threads_.clear();
	queue_.clear();
}

bool eko::ThreadPool::run(Functor && func)
{
	if (threads_.empty())
		return false;

	return queue_.putBack(std::move(func));
}

void eko::ThreadPool::runInThread()
{
	while (running_)
	{
		Functor func;
		if (take(func) && func)
		{
			func();
		}
	}
}

bool eko::ThreadPool::take(Functor& func)
{
	eko::Lock lock(mutex_);
	if (!running_)
		return false;
	return queue_.getFront(func, 1000);
}
