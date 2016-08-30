#include "base/buffer.h"
#include "base/log.h"

#include <string>

using namespace std;
using namespace eko;

static void printBuffer(const char* pch, size_t size)
{
    static const size_t count = 50;

    size_t len = (count>size)?size:count;
    while (len > 0)
    {
        printf("%c", *pch);
        len--;
        pch++;
    }

    if (count < size)
        printf("...\n");
    else printf("\n");
}

static bool checkBuffer(const char* pch0, size_t len, const char* pch1)
{
    if (len != strlen(pch1))
        goto check_error;


    for (unsigned int i=0; i<len; i++)
    {
        if (*(pch0+i) != *(pch1+i))
            goto check_error;
    }
    printf("OK : ");
    printf(pch1);
    printf("\n");
    return true;

check_error:
    printf("ERROR : \n  expect : ");
    printf(pch1);
    printf("\n  result : ");
    printBuffer(pch0, len);
    return false;
}

static bool checkBuffer(const Buffer& buff, const char* str)
{
    const char* pch0 = buff.peek();
    const char* pch1 = str;
    size_t len = buff.readableBytes();

    return checkBuffer(pch0, len, pch1);
}

#define CHECK_BUFFER(a, b) assert(checkBuffer((a), (b)))
#define CHECK_BUFFER2(a, b, c) assert(checkBuffer((a), (b), (c)))

/// A buffer class modeled after muduo Buffer
///
/// @code
/// +-------------------+------------------+------------------+
/// | prependable bytes |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
/// @endcode

void test_buffer()
{
    Buffer buff;
	int size = 605;
	char* data = new char[size];
	memset(data, 11, size);

    LOGI("\n======= test_buffer =======");

	buff.pushBack((BYTE)0x27, 44);
	buff.erase();

	buff.pushBack((BYTE)0x27, 1);

	buff.pushBack((BYTE)0x01, 1);
	buff.pushBack((BYTE)0x00, 3);

	// NALU size   
	buff.pushBack((BYTE)(size >> 24 & 0xff), 1);
	buff.pushBack((BYTE)(size >> 16 & 0xff), 1);
	buff.pushBack((BYTE)(size >> 8 & 0xff), 1);
	buff.pushBack((BYTE)(size & 0xff), 1);

	// NALU data   
	buff.pushBack((const char*)data, size);
	delete[] data;

	/*
    buff.pushFront("hello world");
    CHECK_BUFFER(buff, "hello world");

    buff.eraseBack(60);
    CHECK_BUFFER(buff, "");

    buff.pushBack("1234567");
    CHECK_BUFFER(buff, "1234567");

    buff.pushFront("0-0");
    CHECK_BUFFER(buff, "0-01234567");

    buff.pushBack("abc");
    CHECK_BUFFER(buff, "0-01234567abc");

    buff.eraseFront(2);
    CHECK_BUFFER(buff, "01234567abc");

    char obuf[128];
    int len = buff.takeFront(obuf, 7);
    CHECK_BUFFER(buff, "7abcefg");
    CHECK_BUFFER2(obuf, len, "0123456");

    len = buff.takeBack(obuf, 5);
    CHECK_BUFFER(buff, "7a");
    CHECK_BUFFER2(obuf, len, "bcefg");

    len = buff.takeFront(obuf, 128);
    CHECK_BUFFER(buff, "");
    CHECK_BUFFER2(obuf, len, "7a");
*/
}
