#ifndef BASE_MUTEX_H_
#define BASE_MUTEX_H_

#include "common.h"
#include <mutex>

namespace eko
{
	class Lockable
	{
	public:
		virtual void lock() = 0;
		virtual void unlock() = 0;
	};

	////////////////////////////////////////////////////

	class Lock
	{
	public:
		Lock(Lockable& l) : lock_(l)
		{
			lock_.lock();
		}

		~Lock(void)
		{
			lock_.unlock();
		}

	private:
		Lockable& lock_;
	};

	////////////////////////////////////////////////////

	class Mutex : public Lockable, Noncopyable
	{
	public:
		virtual void lock()
		{
			mutex_.lock();
		}

		virtual void unlock()
		{
			mutex_.unlock();
		}

		bool trylock()
		{
			return mutex_.try_lock();
		}

	private:
		std::recursive_mutex mutex_;
	};
}

#endif
