#ifndef _BASE_BUFFER_H__
#define _BASE_BUFFER_H__ 

#include <string.h>
#include <vector>

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

namespace eko
{
	class Buffer
	{
		static const size_t kMiniSize = 64;

	public:
		Buffer(size_t initSize = kMiniSize);
		Buffer(const char* data, size_t size);
		Buffer(const Buffer& rhs);

		void swap(const Buffer& rhs)
		{
			buffer_.swap(rhs.buffer_);
			std::swap(readerIndex_, rhs.readerIndex_);
			std::swap(writerIndex_, rhs.writerIndex_);
			std::swap(limitSize_, rhs.limitSize_);
		}

		Buffer& operator=(const Buffer& rhs)
		{
			swap(rhs);
			return *this;
		}

		Buffer& operator=(const char* str)
		{
			erase();
			pushBack(str);
			return *this;
		}

		size_t readableBytes() const
		{
			return writerIndex_ - readerIndex_;
		}

		size_t writableBytes() const
		{
			return buffer_.size() - writerIndex_;
		}

		size_t prependableBytes() const
		{
			return readerIndex_;
		}

		const char* peek() const
		{
			return begin() + readerIndex_;
		}

		char* beginWrite()
		{
			return begin() + writerIndex_;
		}

		const char* beginWrite() const
		{
			return begin() + writerIndex_;
		}

		char* beginRead()
		{
			return begin() + readerIndex_;
		}

		const char* beginRead() const
		{
			return peek();
		}

		void erase()
		{
			readerIndex_ = writerIndex_ = 0;
		}

		void eraseBack(size_t len)
		{
			if (len >= readableBytes())
				erase();
			else writerIndex_ -= len;
		}

		void eraseFront(size_t len)
		{
			if (len >= readableBytes())
				erase();
			else readerIndex_ += len;
		}

		void writerIndexMove(int m)
		{
			writerIndex_ += m;
			if (writerIndex_ > buffer_.size())
				writerIndex_ = buffer_.size();
		}

		size_t pushBack(size_t len);
		size_t pushBack(unsigned char ch, size_t len);
		size_t pushBack(const char* data, size_t len);
		size_t pushBack(const char* str)
		{
			return pushBack(str, ::strlen(str));
		}

		size_t pushFront(const char* data, size_t len);
		size_t pushFront(const char* str)
		{
			return pushFront(str, ::strlen(str));
		}

		size_t takeBack(char* buff, size_t size);
		size_t takeFront(char* buff, size_t size);

		void limitSize(int limit = -1);

	private:
		char* begin()
		{
			return &*buffer_.begin();
		}

		const char* begin() const
		{
			return &*buffer_.begin();
		}

		size_t makeSpaceBack(size_t len);
		size_t makeSpaceFront(size_t len);
		void moveData(int span);

	private:
		mutable std::vector<char> buffer_;
		mutable size_t readerIndex_;
		mutable size_t writerIndex_;
		mutable int limitSize_;
	};
}

#endif
