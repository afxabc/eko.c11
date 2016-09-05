#ifndef BASE_SYSTEMTIME_H_
#define BASE_SYSTEMTIME_H_

#include <string>
#include <iostream>

#include "common.h"
#include "log.h"

namespace eko
{
	class MicroSecond
	{
	public:
		MicroSecond(int ms = 0) : microSeconds_(ms) {}

		operator int() const
		{
			return microSeconds_;
		}

	private:
		int microSeconds_;			//∫¡√Î

		friend class Timestamp;
	};

	class Timestamp
	{
	public:
		static Timestamp NOW();
		static Timestamp FOREVER();

		Timestamp()
		{
			microSeconds_ = NOW().microSeconds_;
		}

		Timestamp(UInt64 ms) : microSeconds_(ms)
		{
		}

		inline operator UInt64() const
		{
			return microSeconds_;
		}

		inline UInt64 operator=(UInt64 ms)
		{
			microSeconds_ = ms;
			return microSeconds_;
		}

		inline void operator+=(MicroSecond rhs)
		{
			microSeconds_ += rhs.microSeconds_;
		}

		inline Timestamp& operator+(MicroSecond rhs)
		{
			(*this) += rhs;
			return *this;
		}

		inline MicroSecond operator-(const Timestamp& rhs) const
		{
			return MicroSecond(static_cast<int>(microSeconds_ - rhs.microSeconds_));
		}

		int toString(char* buf, int size) const;
		std::string toString() const;

	private:
		UInt64 microSeconds_;			//∫¡√Î
	};

	class TimeTick		//º∆ ±
	{
	public:
		MicroSecond lapse()
		{
			return Timestamp::NOW() - start_;
		}
	private:
		Timestamp start_;			
	};
}

inline std::ostream& operator<<(std::ostream& os, const eko::Timestamp& tm)
{
	os << tm.toString();
	return os;
}

#endif
