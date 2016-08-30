#include "base/signal.h"
#include "base/log.h"

#include <string>
using namespace eko;

void test_signal()
{
    LOGI("\n======= test_signal =======");

    static Signal signal_;

    LOGD("wait for signal ...");
    bool ret = signal_.wait(1000);
    if (! ret)
        LOGD("wait for signal timeout !!");
    else LOGD("signal ????");


}
