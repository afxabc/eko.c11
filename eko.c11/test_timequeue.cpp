#include "base/timequeue.h"
#include "base/thread.h"
#include "base/log.h"

using namespace std;

using namespace eko;

static TimeQueue<int> queue;
bool test = true;

void getLoop()
{
	int ret;
	while (test)
	{
		int get = queue.getFront(ret, 3000);
		Timestamp now;
		if (get > 0)
			LOGI("%s === %d", now.toString().c_str(), ret);
		else if (get == 0)
			LOGI("%s timeout ...", now.toString().c_str());
		else LOGI("%s queue is null !!!", now.toString().c_str());
	}

    LOGI("\n%s get loop quit.", Timestamp::NOW().toString().c_str());
}

void test_timequeue()
{
	Timestamp now;
    LOGI("\n%s ======= test_timequeue =======", now.toString().c_str());

	int num = 6;
//	queue.insert(num, MicroSecond(num*1000));

	Thread thread_;
	thread_.start(getLoop);

	srand((unsigned int)time(NULL));

	while (test)
	{
		char ch = getchar();
		switch (ch)
		{
		case 'i':
		case 'I':
			num = rand() % 10;
			LOGI("%s --> insert %d s", Timestamp::NOW().toString().c_str(), num);
			queue.insert(num, MicroSecond(num*1000));
			break;
		case 'q':
		case 'Q':
			test = false;
			queue.clear();
			break;
		}
	}

	thread_.stop();

}
