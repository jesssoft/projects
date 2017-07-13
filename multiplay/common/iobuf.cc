//
// 2016.04.15 jesssoft
//
#include <cstring>
#include <cassert>
#include "iobuf.h"

IOBuf::IOBuf(unsigned int buf_size)
: cIO_BUF_SIZE(buf_size)
{
	head_ = 0;
	tail_ = 0;
	error_ = false;
	buf_ = new unsigned char[cIO_BUF_SIZE];
}

IOBuf::~IOBuf()
{
	if (buf_) {
		delete[] buf_;
		buf_ = NULL;
	}
}

unsigned char *
IOBuf::GetRestBufPtr(void)
{
	if (tail_ >= cIO_BUF_SIZE)
		return NULL;

	return &buf_[tail_];
}

unsigned int 
IOBuf::GetRestBufSize(void)
{
	assert(tail_ <= cIO_BUF_SIZE);
	return cIO_BUF_SIZE - tail_;
}

bool 
IOBuf::AddDataSize(unsigned int size)
{
	if (IsError())
		return false;

	if (tail_ + size > cIO_BUF_SIZE) {
		SetError();
		return false;
	}

	tail_ += size;
	return true;
}

unsigned int
IOBuf::GetDataSize(void)
{
	assert(tail_ >= head_);
	return tail_ - head_;
}

unsigned int 
IOBuf::GetMsgSize(void)
{
	assert(IsHeader());
	return GetHeader()->length;
}

IO_HEADER *
IOBuf::GetHeader(void)
{
	assert((head_ + sizeof(IO_HEADER)) <= cIO_BUF_SIZE);
	IO_HEADER *header = reinterpret_cast<IO_HEADER *>(&buf_[head_]);
	return header;
}

IO_HEADER *
IOBuf::GetTailHeader(void)
{
	assert((tail_ + sizeof(IO_HEADER)) <= cIO_BUF_SIZE);
	IO_HEADER *header = reinterpret_cast<IO_HEADER *>(&buf_[tail_]);
	return header;
}

unsigned char *
IOBuf::GetDataPtr(void)
{
	assert(head_ < cIO_BUF_SIZE);
	return &buf_[head_];
}

unsigned char *
IOBuf::GetMsgDataPtr(void)
{
	return GetDataPtr() + sizeof(IO_HEADER);
}

void 
IOBuf::SubDataSize(unsigned int size)
{
	if (IsError())
		return;

	if ((head_ + size) > cIO_BUF_SIZE) {
		SetError();
		return;
	}

	head_ += size;
	assert(head_ <= cIO_BUF_SIZE);
}

void 
IOBuf::Rearrange(void)
{
	if (head_ == 0)
		return;

	if (head_ == tail_) {
		head_ = 0;
		tail_ = 0;
		return;
	}

	unsigned int dataSize = GetDataSize();
	assert(dataSize > 0);

	assert((head_ + dataSize) <= cIO_BUF_SIZE);
	memmove(buf_, &buf_[head_], dataSize);
	head_ = 0;
	tail_ = dataSize;
}

bool 
IOBuf::IsEmpty(void)
{
	if (GetDataSize() == 0)
		return true;

	return false;
}

bool 
IOBuf::IsHeader(void)
{
	if (GetDataSize() < sizeof(IO_HEADER))
		return false;

	return true;
}

bool 
IOBuf::IsMsg(void)
{
	if (!IsHeader())
		return false;

	if (GetDataSize() < (sizeof(IO_HEADER) + GetMsgSize()))
		return false;

	return true;
}

