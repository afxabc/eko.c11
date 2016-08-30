#include "common.h"
#include "astring.h"

using namespace eko;
//////////////////////////////////////////////////////////////////////

const AString operator+(const AString& str1, const AString& str2)
{
	AString str(str1);
	str += str2;
	return str;
}

std::ostream& operator<<(std::ostream& os, const AString& str)
{
	if (str.string() != NULL)
		os << str.string();
	return os;
}

//////////////////////////////////////////////////////////////////////

AString::AString(void)
{
}

AString::AString(const char* str)
{
	if (str != NULL)
	{
		buffer_.pushBack(str, strlen(str));
		buffer_.pushBack((unsigned char)0, 1);
	}
}

AString::~AString(void)
{
}

void AString::copy(const AString& str)
{
	buffer_.erase();
	if (!str.isNull())
		buffer_.pushBack(str.string(), str.length()+1);
}

void AString::operator+=(const AString& str)
{
	if (str.isNull())
		return;

	buffer_.eraseBack(1);
	buffer_.pushBack(str.string(), str.length()+1);
}

void AString::format(const char* sformat, ...)
{
	buffer_.erase();

	if (sformat == NULL)
		return;

	static const int MAX_BUF_SIZE = 2048;
	char buf[MAX_BUF_SIZE];

	va_list ap;
	va_start(ap, sformat);

	int wlen = vsnprintf(buf, MAX_BUF_SIZE, sformat, ap);

	va_end(ap);

	if (wlen > 0)
		buffer_.pushBack(buf, wlen);
}

void AString::trimLeft()
{
	if (isNull())
		return;

	int t = 0;
	BYTE* pch = (BYTE*)string();
	while (*pch != 0)
	{
		if (*pch > 0x20)
			break;
		pch++;t++;
	}
	buffer_.eraseFront(t);
}
	
void AString::trimRight()
{
	if (isNull())
		return;

	int len = buffer_.readableBytes()-1;
	BYTE* pch = (BYTE*)string()+len;

	int t = 0;
	while (t < len)
	{
		if (*pch > 0x20)
			break;
		pch--;t++;
	}
	buffer_.eraseBack(t);
	buffer_.pushBack((unsigned char)0, 1);
}

AString AString::mid(int iFirst, int iCount)
{
	int len = buffer_.readableBytes()-1;
	if (iCount < 0 || iCount > len-iFirst)
		iCount = len-iFirst;

	AString str;
	if (iCount > 0 || iFirst >= 0)
	{
		str.buffer_.pushBack(iCount);
		memcpy(str.buffer_.beginRead(), string()+iFirst, iCount);
		str.buffer_.pushBack((unsigned char)0, 1);
	}

	return str;
}

AString AString::left(int iCount)
{
	return mid(0, iCount); 
}

void AString::erase(int iFirst, int iCount)
{
	int len = buffer_.readableBytes()-1;
	if (iFirst < 0 || iFirst > len)
		return;

	int remain = len-iFirst;
	if (iCount < 0 || iCount >= remain)
		iCount = remain+1;

	if (iFirst == 0)
	{
		buffer_.eraseFront(iCount);
	}
	else if (iCount == remain+1)
	{
		buffer_.eraseBack(iCount);
		if (buffer_.readableBytes() > 0)
			buffer_.pushBack((unsigned char)0, 1);
	}
	else
	{
		char* pFirst = buffer_.beginRead()+iFirst;
		remain = remain+1-iCount;
		for (int i=0; i<remain; ++i)
			*(pFirst+i) = *(pFirst+i+iCount);
		buffer_.eraseBack(iCount);
	}
}

void AString::insert(int iFirst, const AString& str)
{
	int len = buffer_.readableBytes()-1;
	int iCount = str.length();
	if (iCount <= 0 || iFirst < 0 || iFirst > len)
		return;

	buffer_.pushBack(iCount);

	char* pFirst = buffer_.beginRead()+iFirst;
	int remain = len-iFirst+1;			//include 0

	for (int i=remain-1; i>=0; --i)
		*(pFirst+i+iCount) = *(pFirst+i);

	for (int i=iCount-1; i>=0; --i)
		*(pFirst+i) = str[i];
}

void AString::insert(int iFirst, const char ch, int iCount)
{
	int len = buffer_.readableBytes()-1;
	if (iCount <= 0 || iFirst < 0 || iFirst > len)
		return;

	buffer_.pushBack(iCount);

	char* pFirst = buffer_.beginRead()+iFirst;
	int remain = len-iFirst+1;			//include 0

	for (int i=remain-1; i>=0; --i)
		*(pFirst+i+iCount) = *(pFirst+i);

	for (int i=iCount-1; i>=0; --i)
		*(pFirst+i) = ch;
}

int AString::findFirst(const char ch, int iFirst)
{
	if (iFirst < 0 || iFirst >= length())
		iFirst = length()-1;

	char* pFirst = buffer_.beginRead()+iFirst;
	while (iFirst < length())
	{
		if (ch == *pFirst)
			break;
		++pFirst;
		++iFirst;
	}

	if (iFirst >= length())
		return -1;

	return iFirst;
}

int AString::findLast(const char ch, int iLast)
{
	if (iLast < 0 || iLast >= length())
		iLast = length()-1;

	char* pLast = buffer_.beginRead()+iLast;
	while (iLast >= 0)
	{
		if (ch == *pLast)
			break;
		--pLast;
		--iLast;
	}

	if (iLast < 0)
		return -1;

	return iLast;
}

bool AString::sameNoCase(const AString& str) const
{
	if (length() == 0 || str.length() == 0 || length() != str.length())
		return false;

	const char* pSrc = buffer_.beginRead();
	const char* pDst = str.buffer_.beginRead();

	int d = 'A'-'a';
	for (int i=0; i<length(); ++i)
	{
		BYTE c0 = (BYTE)(*(pSrc+i));
		BYTE c1 = (BYTE)(*(pDst+i));
		if (c0 == c1 || (c0 >= 'A' && c0 <= 'Z' && c0-d == c1) || (c0 >= 'a' && c0 <= 'z' && c0+d == c1))
			continue;
		return false;
	}

	return true;
}

