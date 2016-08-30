#include "functorqueue.h"
#include "log.h"

using namespace eko;

void FunctorQueue::post(Functor&& func)
{
	Lock lock(mutex_);
	LOGI("push_back");
	actQueue_.push_back(move(func));
}

UInt32 FunctorQueue::post(Functor&& func, MicroSecond delay)
{
	if (delay <= 0)
	{
		post(move(func));
		return 0;
	}

	Lock lock(mutex_);

	sequence_++;
	if (sequence_ == NULL_SEQUENCE)
		sequence_++;

	LOGI("insert .. ");
	TimerQueue::iterator it = tmQueue_.insert(std::pair<Timestamp, TmFunctor>(Timestamp::NOW()+MicroSecond(delay), TmFunctor(sequence_, move(func))));

	SequenceMap::iterator st = seqMap_.find(sequence_);
	if (st != seqMap_.end())
	{
		LOGW("sequence %d overload, replace !!", sequence_);
		st->second->second.sequence_ = NULL_SEQUENCE;
	}
	
	seqMap_[sequence_] = it;

	return sequence_;
}

bool FunctorQueue::cancel(UInt32 sequence)
{
	Lock lock(mutex_);
	
	SequenceMap::iterator st = seqMap_.find(sequence);
	if (st == seqMap_.end())
	{
		LOGW("cancel inexistence timer functor !! sequence=%d", sequence);
		return false;
	}

	tmQueue_.erase(st->second);
	seqMap_.erase(st);

	return true;
}

MicroSecond FunctorQueue::run()
{
	MicroSecond delay(0);

	if (!tmQueue_.empty())
	{
		Lock lock(mutex_);
		Timestamp now;
		TimerQueue::iterator it = tmQueue_.begin();
		for (; it!=tmQueue_.end(); ++it)
		{
			delay = it->first-now;
			if (delay > 0)
				break;
			actQueue_.push_back(move(it->second.func_));
			seqMap_.erase(it->second.sequence_);
		}
		if (it == tmQueue_.end())
			delay = 0;
		tmQueue_.erase(tmQueue_.begin(), it);
	}

	if (!actQueue_.empty())
	{
		ActiveQueue tmp;
		{
			Lock lock(mutex_);
			actQueue_.swap(tmp);
		}
		ActiveQueue::iterator it = tmp.begin();
		for (; it!=tmp.end(); ++it)
			(*it)();
	}

	return delay;
}

void FunctorQueue::clear()
{
	Lock lock(mutex_);
	tmQueue_.clear();
	seqMap_.clear();
	actQueue_.clear();
	sequence_ = NULL_SEQUENCE;
}

void FunctorQueue::dump()
{
	LOGI("=== FunctorQueue dump ===");
	LOGI("Active queue size = %d", actQueue_.size());
	LOGI("Timer queue size = %d", tmQueue_.size());
	Timestamp now;
	TimerQueue::iterator it=tmQueue_.begin();
	for (UInt32 n=0; it!=tmQueue_.end(); ++it,++n)
	{
		LOGI("[%d] (seq:%d) wait %d ms.", n, it->second.sequence_, (int)(it->first-now));
	}
	LOGI("=== FunctorQueue dump ===\n");
}
