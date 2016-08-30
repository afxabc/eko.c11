#ifndef _BASE_TIME_QUEUE_H__
#define _BASE_TIME_QUEUE_H__

#include <map>
#include "mutex.h"
#include "signal.h"
#include "log.h"

namespace eko
{
	template <typename QDATA>
	class TimeQueue
	{
	public:

		TimeQueue()
		{
		}

		~TimeQueue()
		{
			clear();
		}

		void insert(const QDATA& data, MicroSecond delay)
		{
			insert(data, Timestamp::NOW() + delay);
		}

		void insert(const QDATA& data, Timestamp when)
		{
			Lock lock(mutex_);
			queue_.insert(QPAIR(when, data));
			sGet_.on();
		}

		int getFront(QDATA& data, MicroSecond wait = 0)
		{
			Timestamp start;
			bool ret = 0;
			MicroSecond delay(-1);

			peerFront(delay);
			while (wait > 0)
			{
				if (delay < wait)
				{
					if (delay == 0)
						break;
					else if (delay > 0)
						wait = delay;
				}

				sGet_.wait(wait);
				wait = wait-(Timestamp::NOW() - start);

				if (!peerFront(delay))
					return -1;
			}

			Lock lock(mutex_);
			auto it = queue_.begin();
			if (it != queue_.end())
			{
				delay = it->first - Timestamp::NOW();
				if (delay <= 0)
				{
					data = it->second;
					queue_.erase(it);
					ret = 1;
				}
			}
			else ret = -1;
			
			return ret;
		}

		bool peerFront(MicroSecond& delay)
		{
			bool ret = false;
			Lock lock(mutex_);
			auto it = queue_.begin();
			if (it != queue_.end())
			{
				delay = it->first- Timestamp::NOW();
				ret = true;
			}
			return ret;
		}

		void clear()
		{
			Lock lock(mutex_);
			queue_.clear();
			sGet_.on();
		}

		size_t size()
		{
			return queue_.size();
		}

	private:
		typedef std::multimap<Timestamp, QDATA> QUEUE;
		typedef std::pair<Timestamp, QDATA> QPAIR;
		QUEUE queue_;
		mutable Mutex mutex_;
		Signal sGet_;
	};
}

#endif
