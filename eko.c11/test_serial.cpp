
#include "base/log.h"
#include "base/thread.h"
#include "base/signal.h"
#include "test.h"
#include "base/serial.h"
#include "base/ratecounter.h"
#include <string>

static Signal sg;
static bool run = false;
static bool pause_ = false;
static int epnum = 0;
static UInt32 lost;
static RateCounter rateCounter(1024);
static int waitTime_ = 0;

extern void print_hex_dat(BYTE *dat, const int len);

static void sendThread(SerialPtr sptr)
{
    static const int MAX_BUF = 1024;
    char buf[MAX_BUF+32];

    LOGI("send thread enter.");
    sg.on();
    int num = 0;
    epnum = 0;
	pause_ = false;
	while (run)
    {
		if (pause_)
		{
			sg.wait(1000);
			continue;
		}

        int len = MAX_BUF;
        int sz = len+16;

        int n = 0;
        buf[n++] = 'X';
        buf[n++] = 'Y';
        //*((int*)(buf+n)) = len+16;
        memcpy(buf+n, &sz, 4);
        //LOGI("send packet size=%X [%X] [%X] [%X] [%X] [%X] [%X] ", sz, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
        n+=4;
        //*((int*)(buf+n)) = num;
        memcpy(buf+n, &num, 4);
        MD5 md5(buf, len);
        memcpy(buf+len, md5.digest(), 16);
        len += 16;

        int slen = 0;
        do
        {
            slen = sptr->sendData(buf, len);
            if (slen == 0)
            {
             //   LOGI("send pending ...");
                sg.wait(100);
            }
        }
        while (slen == 0 && run);

        num++;
       //if (num % 10 == 0)

        //sg.wait(1000);
    }
    LOGI("send thread quit.");
}

static Buffer recvBuff;
static void handleRead(char* buf, int len)
{
    assert(len > 0);

	rateCounter.count(len);

    recvBuff.pushBack(buf, len, true);

    buf = recvBuff.beginRead();
    len = recvBuff.readableBytes();

    int span = 0;
    while (len >= 6)
    {
        if (buf[0]!='X' || buf[1]!='Y')
        {
            buf++, span++, len--;
            continue;
        }

        if (span > 0)
            recvBuff.eraseFront(span);

        //LOGI("hit head!! span=%d", span);

        span = 0;
        buf = recvBuff.beginRead();
        len = recvBuff.readableBytes();

        //int sz = *((int*)(buf+2));
        int sz = 0;
        memcpy(&sz, buf+2, 4);
		if (sz <= 0)
			break;
        //LOGI("packet size=%X [%X] [%X] [%X] [%X] [%X] [%X] ", sz, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);
        if (len < sz)
            break;

        //int num = *((int*)(buf+6));
        int num = 0;
        memcpy(&num, buf+6, 4);
 //       if (num != epnum)
 //           LOGE("recv num expect %d but %d.", epnum, num);
        epnum = num+1;
  //      if (num % 10 == 0)
        //LOGI("recv packet %d .", num);

        MD5 md5(buf, sz-16);
        if (memcmp(buf+sz-16, md5.digest(), 16) != 0)
            LOGE("recv num %d MD5 error !!!", num);

        recvBuff.eraseFront(sz);
        buf = recvBuff.beginRead();
        len = recvBuff.readableBytes();

    }
//	else LOGI("recv from %s : %d", addr.toString().c_str(), len);
}

static void handleRead2(char* buf, int len)
{
	static int total = 0;
	total += len;
	LOGI("tty recv : %d(%d)", len, total);
	print_hex_dat((BYTE*)buf, len);
}


#ifndef WIN32
/* ECS-IFP-IPMI-A01 �ڲ���Ԫͨ��Э�� FPGA��ز��� ��ioctl���� */
struct fpga_param_struct
{
	unsigned char ifType;			//�ӿ�����

	unsigned char tdmChnl;			//�ýӿڶ�Ӧ��TDMͨ��
	unsigned char slotStart;			//ռ�õ���ʼʱ϶
	unsigned char slotNum;			//ռ�õ�ʱ϶��
	
	unsigned char xMode : 1;			//֧��XON/XOFF
	unsigned char dtrMode : 1;		//֧��DTR/DSR
	unsigned char rtsMode : 1;		//֧��RTS/CTS
	unsigned char synMode : 2;		//ͬ��/�첽ģʽ��00b=�첽ģʽ��01b=ͬ��master��10b=ͬ��slave
	unsigned char noused1 : 2;		//����
	unsigned char dteMode : 1;		//DTE/DCE

	unsigned int nBaudrate;	//������

	unsigned char nParity : 3;		//У�鷽ʽ��000b=��У�顢011b=��У�顢010b=żУ�顢101b=���У�顢100b=�պ�У��
	unsigned char nStopbits : 2;		//ֹͣλλ����00b= 1��ֹͣλ��01b=1.5��ֹͣλ��10b=2��ֹͣλ
	unsigned char nDatabits : 3;		//����λλ����000b= 8λ����λ��001b= 7λ����λ��010b= 6λ����λ��011b= 5λ����λ��1xxb= ����
}
__attribute__((packed));

#define TIOCSETFPGA	0x54A0	/* ����FPGA���� */

enum EnumLinkType
{
	LINK_TYPE_RS232C	= 0x10,		// RS232C����
	LINK_TYPE_RS422,
	LINK_TYPE_RS449,
	LINK_TYPE_RS485,
};
#endif

void test_serial(const char* str)
{
	const char* ttyName = str;
    if (!str)
    {
        printf("enter tty name : ");
        char line[64]; // room for 20 chars + '\0'
        fgets(line, sizeof(line)-1, stdin);
		if (strlen(line) >= 4)
		{
			for (int i=0; i<strlen(line); i++)
			{
				if ((int)line[i] == 10 || (int)line[i] == 13)
					line[i] = 0;
				if (line[i] == 0)
					break;
			}
			ttyName = line;
		}
#ifdef WIN32
		else ttyName = "COM4";
#else
		else ttyName = "/dev/ttyS1";
#endif
    }

    SerialPtr sptr(new Serial());
	if (!sptr->open(ttyName, Serial::BR_115200))
	{
		LOGE("can not open %s !", ttyName);
		return;
	}
	
	LOGI("%s openned .", ttyName);
    sptr->setReadCallback(boost::bind(&handleRead, _1, _2));
//    sptr->setReadCallback(boost::bind(&handleRead2, _1, _2));

#ifndef WIN32
	struct fpga_param_struct params;
	params.ifType = LINK_TYPE_RS232C;
	params.rtsMode = 0;
	params.synMode = 0;
	params.dteMode = 0;
	params.nBaudrate = 115200;
	params.nParity = 0;
	params.nStopbits = 0;
	params.nDatabits = 0;
	ioctl(sptr->fd(), TIOCSETFPGA, &params);
#endif

    Thread thread;

    bool test = true;
    while (test)
    {
        char ch = getchar();
        switch (ch)
        {
        case 'o':
        case 'O':
            run = true;
			waitTime_ = 0;
            thread.start(boost::bind(&sendThread, sptr));
            break;
        case 'l':
        case 'L':
        {
			float rate = (float)rateCounter.bytesPerSecond()/1000;
            char c = 'k';
            if (rate > 1024)
                rate /= 1024, c = 'M';

			if (epnum > 0)
				LOGI("rate=%.2f%c  lost=%.1f%%(%d/%d)", rate, c, (float)lost*100/epnum, lost, epnum);
			else LOGI("rate=%.2f%c", rate, c);

        }
        break;
        case 'c':
        case 'C':
            run = false;
			waitTime_ = 0;
            sg.on();
            thread.stop();
            break;
        case 's':
        case 'S':
			waitTime_ = -1;
            sg.on();
            break;
        case 'p':
        case 'P':
            pause_ = !pause_;
            sg.on();
			if (pause_)
				LOGI("send pause ...");
			else LOGI("send continue ...");
            break;
        case 't':
        case 'T':
			{
			static const int SEND_SIZE = 1024;
			char buf[SEND_SIZE];
			for (int i=0; i<SEND_SIZE; i++)
				buf[i] = i;
            int slen = sptr->sendData(buf, SEND_SIZE);
            if (slen == 0)
                LOGI("send pending ...");
			else LOGI("tty send : %d", slen);
            break;
			}
        case 'q':
        case 'Q':
            test = false;
            run = false;
            sg.on();
            thread.stop();
            break;
        }
    }

    sptr.reset();

}
