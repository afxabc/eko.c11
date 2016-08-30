#include "functorloop.h"

using namespace eko;

FunctorLoop::FunctorLoop() : threadId_(0), run_(false)
{
	loopFun_ = std::bind(&FunctorLoop::defaultLoop, this);
	waitFun_ = std::bind(&FunctorLoop::defaultWait, this, std::placeholders::_1);
	wakeupFun_ = std::bind(&FunctorLoop::defaultWakeup, this);
}

FunctorLoop::~FunctorLoop()
{
	quitLoop();
}

void FunctorLoop::defaultLoop()
{
	signal_.off();

	static const int TM = 5000;
	MicroSecond delay(TM);

	while(run_)
	{
		waitFun_(delay);
		delay = queue_.run();
		if (delay == 0)
			delay = TM;
	}
}

void FunctorLoop::defaultWait(MicroSecond delay)
{
	signal_.wait(delay);
}

void FunctorLoop::defaultWakeup()
{
	signal_.on();
}

UInt32 FunctorLoop::runInLoop(Functor&& func, MicroSecond delay)
{ 
	UInt32 ret = queue_.post(move(func), delay);
	wakeupFun_();
	return ret;
}

void FunctorLoop::loop()
{
	assert(!run_ && threadId_== 0);
	threadId_ = Thread::self();

	run_ = true;
//	queue_.clear();

	if (loopFun_)
		loopFun_();

	run_ = false;
	queue_.clear();
	threadId_ = 0;
}

void FunctorLoop::quitLoop()
{
	if (run_)
	{
		run_ = false;
		wakeupFun_();
		thread_.stop();
	}
}
