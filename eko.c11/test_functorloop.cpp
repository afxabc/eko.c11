#include "base/functorloop.h"
#include "base/log.h"

#include <string>

using namespace std;
using namespace eko;

static Signal signal_;

static void callback(const string& str)
{
    LOGD("== Callback print : %s", str.c_str());
    signal_.on();
}

static void CHECK(bool condition, const char* sformat, ...)
{
    if (!condition)
    {
        va_list ap;
        va_start(ap, sformat);
        LOGE(sformat, ap);
        va_end(ap);
    }
    assert(condition);
}

void test_functorloop()
{
    LOGI("\n======= test_functorloop =======");

    FunctorLoop loop;
    bool ret;

    loop.loopInThread();

    LOGD("run in loop right now.");
    loop.runInLoop(std::bind(callback, move((string("hello world.")))));
    ret = signal_.wait(10);
    CHECK(ret, "callback timeout!!");

    LOGD("run in loop after 2s. ");
    loop.runInLoop(std::bind(callback, move(string("nice day."))), 2000);
    ret = signal_.wait(2010);
    CHECK(ret, "callback timeout!!");

    LOGD("run in loop after 3s. ");
    loop.runInLoop(std::bind(callback, move(string("this is 3."))), 3000);
    LOGD("run in loop after 2s. ");
    loop.runInLoop(std::bind(callback, move(string("this is 2."))), 2000);
    LOGD("run in loop after 1s. ");
    UInt32 seq = loop.runInLoop(std::bind(callback, move(string("this is 1."))), 1000);

    LOGI("cancel 1 (seq=%d).", seq);
    ret = loop.cancel(seq);
    CHECK(ret, "cancel error!");

    ret = signal_.wait(1010);
    CHECK(!ret, "unexpect signal_ in 1s!");

    ret = signal_.wait(1010);
    CHECK(ret, "callback timeout!!");

    ret = signal_.wait(1010);
    CHECK(ret, "callback timeout!!");

    loop.quitLoop();
}
