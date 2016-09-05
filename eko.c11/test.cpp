#include "test.h"
#include "base/log.h"

using namespace eko;

extern void test_queue();
extern void test_timequeue();
extern void test_buffer();
extern void test_string();
extern void test_functor();
extern void test_functorloop();
extern void test_signal();
extern void test_threadpool();


void print_hex_dat(BYTE *dat, const int len)
{
	printf("-----------------------------------------------\n");
	int llen = len/16;
	BYTE* pl = dat;
	for(int i=0; i<llen; i++)
	{
		printf("%.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X %.2X \n", 
				(int)pl[0], (int)pl[1], (int)pl[2], (int)pl[3], (int)pl[4], (int)pl[5], (int)pl[6], (int)pl[7], 
				(int)pl[8], (int)pl[9], (int)pl[10], (int)pl[11], (int)pl[12], (int)pl[13], (int)pl[14], (int)pl[15]);
		pl += 16;
	}
	llen = len%16;
	for(int i=0; i<llen; i++)
	{
		printf("%.2X ",(int)pl[0]);
		pl++;
	}
	if (llen > 0)
		printf("\n-----------------------------------------------\n");
	else printf("-----------------------------------------------\n");
}

//////////////////////////////////////////////////////////////////
/*
报头：X+Y	[2]
长度：sz	[4]
数据：		[sz-2-4-16]
报尾：md5	[16]
*/
int FrameBuffer::unwrap(char* outBuf, int bufSize)
{
	int ret = 0;

    while (buffer_.readableBytes() >= 6)
    {
        char* buf = buffer_.beginRead();
        int len = buffer_.readableBytes();

        if (buf[0]!='X' || buf[1]!='Y')
        {
            buffer_.eraseFront(1);
            continue;
        }

        int sz = 0;
        memcpy(&sz, buf+2, 4);
		if (sz <= 0)
		{
            buffer_.eraseFront(6);
			continue;
		}

        if (len < sz)
            break;

        MD5 md5(buf, sz-16);
        if (memcmp(buf+sz-16, md5.digest(), 16) != 0)
		{
			buffer_.eraseFront(sz);
			LOGE("FrameBuffer unwrap MD5 error !!!");
			continue;
		}

		int reqSzie = sz-2-4-16;
		if (bufSize >= reqSzie)
		{
			memcpy(outBuf, buf+6, reqSzie);
			ret = reqSzie;
			buffer_.eraseFront(sz);
			break;
		}
		else
		{
			LOGW("FrameBuffer unwrap: out buffer too small !!!");
			ret = -reqSzie;
			break;
		}    
    }

	return ret;
}

//////////////////////////////////////////////////////////////////

int main(int argc, const char* argv[])
{
    LOGLEVEL(DEBUG);

	srand((unsigned int)time(NULL));
//  test_queue();
//	test_timequeue();
//	test_buffer();
//	test_string();
//	test_functor();
//	test_functorloop();
//	test_signal();
//	test_udp(argv[1]);
//	test_tcp(argv[1]);
//	test_serial(argv[1]);
	test_threadpool();

#ifdef WIN32
    system("pause");
#endif

    return 0;
}
