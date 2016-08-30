#ifndef _BASE_QUEUE_H__
#define _BASE_QUEUE_H__

#include <deque>
#include "mutex.h"
#include "signal.h"
#include "log.h"

namespace eko
{
	template <typename QDATA>
	class Queue
	{
	public:
		Queue(unsigned int max = 0xffffffff) : sPut_(true)
		{
			setMax(max);
		}

		bool putBack(const QDATA& data, UInt32 tm_wait = 0)
		{
			return put(data, tm_wait, true);
		}

		bool putFront(const QDATA& data, UInt32 tm_wait = 0)
		{
			return put(data, tm_wait, false);
		}

		bool getBack(QDATA& data, UInt32 tm_wait = 0)
		{
			return get(data, tm_wait, true);
		}

		bool getFront(QDATA& data, UInt32 tm_wait = 0)
		{
			return get(data, tm_wait, false);
		}

		void clear();

		void setMax(unsigned int max) {
			max_ = max;
			if (max_ == 0)
				max_ = 1;
		}

		void signalPut() {
			sPut_.on();
		}

		void signalGet() {
			sGet_.on();
		}

		void signalAll() {
			sGet_.on();
			sPut_.on();
		}

		size_t size() {
			return queue_.size();
		}

	private:
		bool put(const QDATA& data, UInt32 tm_wait, bool back);
		void erase(size_t len, bool back);
		bool get(QDATA& data, UInt32 tm_wait, bool back);

	private:
		typedef std::deque<QDATA> QUEUE;
		QUEUE queue_;
		mutable Mutex mutex_;
		Signal sPut_, sGet_;
		unsigned int max_;
	};

	template <typename QDATA>
	bool Queue<QDATA>::put(const QDATA& data, UInt32 tm_wait, bool back)
	{
		if (!sPut_.wait(tm_wait))
			return false;

		Lock lock(mutex_);
		if (queue_.size() >= max_)
		{
			LOGW("PUT : queue is full !!!");
			return false;
		}

		if (back)
			queue_.push_back(data);
		else queue_.push_front(data);

		sGet_.on();
		if (queue_.size() < max_)
			sPut_.on();

		return true;
	}

	template <typename QDATA>
	bool Queue<QDATA>::get(QDATA& data, UInt32 tm_wait, bool back)
	{
		if (!sGet_.wait(tm_wait))
			return false;

		Lock lock(mutex_);
		if (queue_.empty())
		{
			LOGW("GET : queue is empty !!!");
			return false;
		}

		if (back)
		{
			data = queue_.back();
			queue_.pop_back();
		}
		else
		{
			data = queue_.front();
			queue_.pop_front();
		}

		if (queue_.size() < max_)
			sPut_.on();
		if (!queue_.empty())
			sGet_.on();

		return true;
	}

	template <typename QDATA>
	void Queue<QDATA>::clear()
	{
		Lock lock(mutex_);
		queue_.clear();
		sPut_.on();
		sGet_.off();
	}
}
#endif
