#include "base/thread.h"
#include "base/threadpool.h"
#include "base/timestamp.h"
#include "base/log.h"

#include <string>
using namespace eko;

void long_count(int count)
{
	TimeTick tick;

	LOGI("*** 0x%.8X : COUNT %d START.", Thread::self(), count);

	float d = 1.0f;
	srand((unsigned int)time(NULL));
	int cn = count * 10000000;
	while (cn-- > 0)
	{
		if ((rand()%2) == 1)
			d += expf(rand()%10);
		else d -= expf(rand()%10);
	}

	LOGI("### 0x%.8X : COUNT %d FIN in %.2f s d=%.2f ", Thread::self(), count, tick.lapse() / 1000.0f, d);
}

void test_threadpool()
{
	LOGI("\n======= test_threadpool =======");

	ThreadPool pool;
	pool.start(10);

	bool test = true;
	while (test)
	{
		char ch = getchar();
		switch (ch)
		{
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			pool.run([=]() {long_count(ch - '0'); });
			break;
		case 'q':
		case 'Q':
			test = false;
			break;
		}
	}

	pool.stop();
}
