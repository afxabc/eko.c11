#include "log.h"

using namespace eko;

void defaultPrint(Log::LEVEL level, const char* sformat)
{
	puts(sformat);
}

Log& LOG_ =  Log::defaultLog();

Log& Log::defaultLog()
{
	static Log log;
	return log;
}

Log::Log()
	: level_(L_INFO)
	, print_([](Log::LEVEL level, const char* sformat) { defaultPrint(level, sformat); })
	, defPrint_([](Log::LEVEL level, const char* sformat) { defaultPrint(level, sformat); })
{
}

void Log::print(LEVEL level, const char* sformat, ...)
{
	static const int LOG_BUF_MAX = 1024;
	char buf[LOG_BUF_MAX+1];

	va_list ap;
	va_start(ap, sformat);
	int len = 0;
	/*
	if (L_INFO != level)
		len = Timestamp::NOW().toString(buf, LOG_BUF_MAX);
		*/
	len += vsnprintf(buf+len, LOG_BUF_MAX-len, sformat, ap);
	buf[len] = 0;
	va_end(ap);

	Lock lock(mutex_);
	if (print_)
		print_(level, buf);
}

