#ifndef BASE_ASTRING_H_
#define BASE_ASTRING_H_

#include "buffer.h"
#include <iostream>

namespace eko
{
	class AString
	{
	public:
		AString(void);
		AString(const char* str);
		AString(const AString& str)
		{
			copy(str);
		}

		~AString(void);

		const AString& operator=(const AString& str)
		{
			copy(str);
			return *this;
		}

		void swap(const AString& rhs)
		{
			buffer_.swap(rhs.buffer_);
		}

		void operator+=(const AString& str);
		void format(const char* sformat, ...);
		void trimLeft();
		void trimRight();
		AString mid(int iFirst, int iCount = -1);
		AString left(int iCount);
		void erase(int iFirst, int iCount = -1);
		void insert(int iFirst, const AString& str);
		void insert(int iFirst, const char ch, int iCount = 1);
		bool sameNoCase(const AString& str) const;
		int findFirst(const char ch, int iFirst = 0);
		int findLast(const char ch, int iLast = -1);

		bool operator>(const AString& str) const
		{
			return (strcmp(string(), str.string()) > 0);
		}

		bool operator<(const AString& str) const
		{
			return (strcmp(string(), str.string()) < 0);
		}

		bool operator==(const AString& str) const
		{
			return (strcmp(string(), str.string()) == 0);
		}

		const char operator[](int pos) const
		{
			if (isNull() || pos < 0 || pos >= size())
				return 0;

			return (buffer_.beginRead())[pos];
		}

		void setAt(int pos, const char ch)
		{
			if (isNull() || pos < 0 || pos >= size())
				return;

			(buffer_.beginRead())[pos] = ch;
		}

		operator const char*() const
		{
			return string();
		}

		const char* string() const
		{
			if (isNull())
				return NULL;
			return buffer_.beginRead();
		}

		char* string()
		{
			if (isNull())
				return NULL;
			return buffer_.beginRead();
		}

		int size() const
		{
			return buffer_.readableBytes();
		}

		int length() const
		{
			//	return strlen(string());
			if (size() > 0)
				return size() - 1;
			return 0;
		}

		bool isNull() const
		{
			return (buffer_.readableBytes() == 0);
		}

	private:
		void copy(const AString& str);

	private:
		Buffer buffer_;
	};

}
	
extern const eko::AString operator+(const eko::AString& str1, const eko::AString& str2);
extern std::ostream& operator<<(std::ostream& os, const eko::AString& str);

#endif
