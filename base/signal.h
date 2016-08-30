#ifndef BASE_SIGNAL_H_
#define BASE_SIGNAL_H_

#include "common.h"
#include "timestamp.h"

namespace eko
{
	class Signal : Noncopyable
	{
#ifndef WIN32
		typedef pthread_cond_t HANDLE;
#endif

	public:
		Signal(bool on = false, bool auto_off = true);
		~Signal();

		void wait();			//ÎÞÏÞµÈ´ý
		bool wait(MicroSecond delay);

		void on();
		void off();

	private:
		bool auto_;
		HANDLE handle_;
		bool signal_;
#ifndef WIN32
		pthread_mutex_t mutex_;
#endif

	};
}

#endif
