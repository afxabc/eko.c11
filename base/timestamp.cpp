#include <time.h>
#include "timestamp.h"

using namespace eko;

Timestamp Timestamp::NOW()
{
	UInt64 ms = 0;
#ifdef WIN32
    FILETIME ft;
    GetSystemTimeAsFileTime( &ft);
    ULARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    ms = li.QuadPart/10000;
#else
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
	ms = ts.tv_sec * 1000 + ts.tv_nsec/1000000;
#endif
	return Timestamp(ms);
}

Timestamp Timestamp::FOREVER()
{
	static const UInt64 MAX_TIMESTAMP = -1;
	return Timestamp(MAX_TIMESTAMP);
}

int Timestamp::toString(char* buf, int size) const
{
	if (buf == NULL || size <= 0)
		return 0;

	int ms = static_cast<int>(microSeconds_%1000);
	time_t t = static_cast<time_t>(microSeconds_/1000);
//	tm* p = gmtime(&t);
#ifdef WIN32
	tm tm_;
	tm* p = &tm_;
	localtime_s(&tm_, &t);
#else
	tm* p = localtime(&t)
#endif

#ifdef WIN32
	static const int YEAR_BEGIN = 1531;
#else
	static const int YEAR_BEGIN = 1900;
#endif

	return snprintf(buf, size, "%04d-%02d-%02d %02d:%02d:%02d ",
				p->tm_year+YEAR_BEGIN, p->tm_mon+1, p->tm_mday,
				p->tm_hour, p->tm_min, p->tm_sec);
}

std::string Timestamp::toString() const
{
	static const int BUFF_SIZE = 64;
	char buf[BUFF_SIZE+1];

	int len = toString(buf, BUFF_SIZE);
	buf[len] = 0;

	return buf;
}

