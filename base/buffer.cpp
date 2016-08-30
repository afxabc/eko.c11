#include "common.h"
#include "buffer.h"

using namespace eko;

Buffer::Buffer(size_t initSize) : buffer_(initSize), readerIndex_(0), writerIndex_(0), limitSize_(-1)
{
}

Buffer::Buffer(const char* data, size_t size) : buffer_((size>kMiniSize)?size:kMiniSize), readerIndex_(0), writerIndex_(size), limitSize_(-1)
{
	std::copy(data, data+size, buffer_.begin());
}

Buffer::Buffer(const Buffer& rhs) : readerIndex_(0), writerIndex_(0), limitSize_(-1)
{
	swap(rhs);
}

void Buffer::moveData(int span)
{
	if (span == 0)
		return;

	int rindex = readerIndex_;
	assert(rindex+span >= 0 && writerIndex_+span <= buffer_.size());

	::memmove(beginRead()+span, beginRead(), readableBytes());
	writerIndex_ += span;
	readerIndex_ += span;
}

size_t Buffer::makeSpaceBack(size_t len)
{
	if (limitSize_ >= 0)
	{
		size_t total = writableBytes() + readableBytes();
		if (total >= (size_t)limitSize_)
			return 0;
		len = limitSize_ - total;
	}

	if (writableBytes() + prependableBytes() < len )
	{
		// FIXME: move readable data
		buffer_.resize(writerIndex_+len);
	}
	else
	{
		// move readable data to the front, make space inside buffer
		int rindex = readerIndex_;
		assert(rindex >= 0);
		moveData(-rindex);
	}

	return len;
}

size_t Buffer::makeSpaceFront(size_t len)
{
	if (limitSize_ >= 0)
	{
		size_t total = writableBytes() + readableBytes();
		if (total >= (size_t)limitSize_)
			return 0;
		len = limitSize_ - total;
	}

	if (writableBytes() + prependableBytes() < len )
	{
		buffer_.resize(writerIndex_+len);
	}
	// move readable data 
	moveData(len-prependableBytes());

	return len;
}

size_t Buffer::pushBack(size_t len)
{
	if (len == 0)
		return 0;

	if (writableBytes() < len)
	{
		len = makeSpaceBack(len);
	}
	writerIndex_ += len;
	return len;
}

size_t Buffer::pushBack(unsigned char ch, size_t len)
{
	if (len == 0)
		return 0;

	if (writableBytes() < len)
	{
		len = makeSpaceBack(len);
	}
	memset(beginWrite(), ch, len);
	writerIndex_ += len;
	return len;
}

size_t Buffer::pushBack(const char* data, size_t len)
{
	if (data == NULL || len == 0)
		return 0;

	if (writableBytes() < len)
	{
		len = makeSpaceBack(len);
	}
	std::copy(data, data+len, beginWrite());
	writerIndex_ += len;
	return len;
}

size_t Buffer::pushFront(const char* data, size_t len)
{
	if (data == NULL || len == 0)
		return 0;

	if (prependableBytes() < len)
	{
		len = makeSpaceFront(len);
	}
	std::copy(data, data+len, beginRead()-len);
	readerIndex_ -= len;
	return len;
}

size_t Buffer::takeBack(char* buff, size_t size)
{
	if (buff == NULL || size == 0)
		return 0;

	size_t len = (readableBytes()>size)?size:readableBytes();

	std::copy(beginWrite()-len, beginWrite(), buff);
	eraseBack(len);

	return len;
}

size_t Buffer::takeFront(char* buff, size_t size)
{
	if (buff == NULL || size == 0)
		return 0;

	size_t len = (readableBytes()>size)?size:readableBytes();

	std::copy(beginRead(), beginRead()+len, buff);
	eraseFront(len);

	return len;
}

void Buffer::limitSize(int limit)
{
	limitSize_ = limit;
}
