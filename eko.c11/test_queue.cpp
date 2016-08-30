#include "base/queue.h"
#include "base/thread.h"
#include "base/log.h"

using namespace std;
using namespace eko;

static bool test = false;
static const UInt32 QSIZE = 3;
static Queue<string> queue(QSIZE);

static string itos(int n)
{
    char buf[128];
    snprintf(buf, 128, "-%d", n);
    return string(buf);
}

static void writer(const char* name, UInt32 span)
{
    int count = 0;
    while (test)
    {
        string str(name);
        str += itos(count);
        LOGD("writer %s -> %s trying .....", name, str.c_str());
        if (queue.putBack(str, span))
        {
            LOGD("writer %s -> %s [%d].", name, str.c_str(), queue.size());
            count++;
//			Thread::sleep(span);
        }
        else LOGD("writer %s timeout !!", name);
    }

    LOGI("writer %s quit.", name);
}

static void reader(const char* name, UInt32 span)
{
    while (test)
    {
        string str;
        if (queue.getFront(str, span))
        {
            LOGD("reader %s === %s [%d].", name, str.c_str(), queue.size());
            Thread::sleep(span);
        }
        //	else LOGI("reader %s timeout !!", name);
    }

    LOGI("reader %s quit.", name);
}

void test_queue()
{
    LOGI("\n======= test_queue =======");

    test = true;
    Thread W1;
    W1.start(std::bind(writer, "W1", 3000));

    Thread R1, R2;
    R1.start(std::bind(reader, "R1", 3000));
    R2.start(std::bind(reader, "R2", 3200));

    int count = 0;
    const char* name = "W2";
    while (test)
    {
        string str(name);
        str += itos(count);
        char ch = getchar();
        switch (ch)
        {
        case 'w':
        case 'W':
            if (queue.putBack(str, 0))
            {
                LOGD("writer %s -> %s [%d].", name, str.c_str(), queue.size());
                count++;
            }
            else LOGD("writer %s timeout !!", name);
            break;
        case 'c':
        case 'C':
            queue.clear();
            break;
        case 'q':
        case 'Q':
            test = false;
            queue.clear();
            break;
        }
    }

    W1.stop();
    R1.stop();
    R2.stop();
}
