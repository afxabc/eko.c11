#ifndef _LOG_HEADER__
#define _LOG_HEADER__

#include "common.h"
#include "mutex.h"


namespace eko
{
	class Log : Noncopyable
	{
	public:
		enum LEVEL
		{
			L_ERROR = 0,
			L_WARN,
			L_INFO,
			L_DEBUG,
			L_NONE,
		};

		typedef std::function<void(LEVEL, const char*)> LogPrint;

		static Log& defaultLog();

		inline void setLevel(LEVEL level)
		{
			level_ = level;
		}

		inline bool checkLevel(LEVEL level)
		{
			return level_ >= level;
		}

		template<class R>
		inline void setPrint(R r)
		{
			Lock lock(mutex_);
			print_ = boost::bind(r, _1, _2);
		}

		template<class R, class T>
		inline void setPrint(R r, T t)
		{
			Lock lock(mutex_);
			print_ = std::bind(r, t, std::placeholders::_1, std::placeholders::_2);
		}

		inline void setPrint()
		{
			Lock lock(mutex_);
			print_ = defPrint_;
		}

		void print(LEVEL level, const char* sformat, ...);

	private:
		Log();

	private:
		LEVEL level_;
		LogPrint print_;
		LogPrint defPrint_;
		Mutex mutex_;
	};

}

extern eko::Log& LOG_;

inline void LOGPRINT()
{
	LOG_.setPrint();
}

template<class R>
inline void LOGPRINT(R r)
{
	LOG_.setPrint(r);
}

template<class R, class T>
inline void LOGPRINT(R r, T t)
{
	LOG_.setPrint(r, t);
}

#define LOGLEVEL(x) LOG_.setLevel(eko::Log::L_##x)

#define LOGI(...)	\
	do	\
	{	\
		if (LOG_.checkLevel(eko::Log::L_INFO))	\
			LOG_.print(eko::Log::L_INFO, __VA_ARGS__);	\
	}while(0)

#define LOGD(...)	\
	do	\
	{	\
		if (LOG_.checkLevel(eko::Log::L_DEBUG))	\
			LOG_.print(eko::Log::L_DEBUG, __VA_ARGS__);	\
	}while(0)

#define LOGW(...)	\
	do	\
	{	\
		if (LOG_.checkLevel(eko::Log::L_WARN))	\
			LOG_.print(eko::Log::L_WARN, __VA_ARGS__);	\
	}while(0)

#define LOGE(...)	\
	do	\
	{	\
		if (LOG_.checkLevel(eko::Log::L_ERROR))	\
			LOG_.print(eko::Log::L_ERROR, __VA_ARGS__);	\
	}while(0)

#endif
