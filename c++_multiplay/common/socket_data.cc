//
// 2016.04.15 jesssoft
//

#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include "socket_data.h"
#include "msg.h"

#define FAKE_CHECKSUM	0x01

using namespace std;

SocketData::SocketData(int fd, int socket_key, unsigned int iobuf_size)
: cIO_BUF_SIZE(iobuf_size), in_(iobuf_size), out_(iobuf_size)
{
	fd_ 		= fd;
	socket_key_	= socket_key;
	data_		= NULL;
	shutdown_ 	= false;
}

SocketData::~SocketData()
{
	Close();
}

bool 
SocketData::IsValidMsgComing(void)
{
	if (!in_.IsHeader())
		return true;

	unsigned int msg_size = in_.GetMsgSize();

	if (msg_size > Msg::cDEFAULT_BUFMAX)
		return false;

	//
	// FAKE_CHECKSUM is not checksum but that makes this sample code simple!
	//
	IO_HEADER *iohdr = in_.GetHeader();
	if (iohdr->checksum != FAKE_CHECKSUM)
		return false;

	return true;
}

bool 
SocketData::IsValidMsg(void)
{
	if (!IsMsg())
		return false;

	return true;
}
	
bool 
SocketData::IsMsg(void)
{
	if (!in_.IsHeader())
		return false;

	if (!in_.IsMsg())
		return false;

	return true;
}

bool 
SocketData::GetMsg(Msg *msg, bool peek)
{
	if (IsMsg() == false)
		return false;

	unsigned int msg_size = in_.GetMsgSize();
	msg->SetMsgData(in_.GetMsgDataPtr(), msg_size);

	if (peek == false)
		in_.SubDataSize(msg_size + sizeof(IO_HEADER));

	return true;
}

void
SocketData::Rearrange(void)
{
	in_.Rearrange();
}

int
SocketData::Read(void)
{
	if (IsNullFd())
		return -1;

	if (in_.IsError())
		return -1;

	if (in_.GetRestBufSize() <= 0)
		return -1;

	int nread = recv(GetFd(), in_.GetRestBufPtr(), in_.GetRestBufSize(), 0);

	if (nread < 0) {
		if (errno == EWOULDBLOCK)
			return 0;
		else
			return -1;
	} else if (nread == 0) {
		// Socket closed.
		return -1;
	}

	in_.AddDataSize(nread);

	if (in_.IsError())
		return -1;

	return nread;
}

void 
SocketData::Shutdown(void)
{
	if (IsNullFd() || IsShutdown())
		return;

	// half closing.
	shutdown(GetFd(), SHUT_RDWR);
	shutdown_ = true;
}

void
SocketData::Close(void)
{
	if (IsNullFd())
		return;

	close(fd_);
	fd_ = -1;
	shutdown_ = true;
}

unsigned int 
SocketData::GetIncomingDataSize(void)
{
	unsigned int data_size = in_.GetDataSize();
	return data_size;
}

unsigned int 
SocketData::GetOutgoingDataSize(void)
{
	unsigned int data_size = out_.GetDataSize();
	return data_size;
}

int 
SocketData::SendMsg(Msg &msg)
{
	if (IsNullFd() || IsShutdown())
		return -1;

	// There is enough buffer to store msg?
	int need_buf_size = msg.GetSize() + sizeof(IO_HEADER);
	int rest_buf_size = out_.GetRestBufSize();

	if (rest_buf_size < need_buf_size) {
		// Try sending existing data in buffer.
		if (out_.GetDataSize() > 0) {
			int ret = Flush();
			if (ret < 0) {
				Shutdown();
				return -1;
			}
		} else {
			Shutdown();
			return -1;
		}

		// Try checking whether enough buffer one more.
		rest_buf_size = out_.GetRestBufSize();

		if (rest_buf_size < need_buf_size) {
			Shutdown();
			return -1;
		}
	}

	// Storing msg data into io buffer.
	IO_HEADER *header = out_.GetTailHeader(); 

	header->checksum = FAKE_CHECKSUM;
	header->length = msg.GetSize();

	out_.AddDataSize(sizeof(IO_HEADER));
	memcpy(out_.GetRestBufPtr(), msg.GetBufPtr(), msg.GetSize());
	out_.AddDataSize(msg.GetSize());

	if (out_.IsError()) {
		Shutdown();
		return -1;
	}

	return 0;
}

int 
SocketData::Flush(void)
{
	if (IsNullFd() || IsShutdown())
		return -1;

	// Nothing to do.
	if (out_.IsEmpty())
		return 0;

	int ret = send(GetFd(), out_.GetDataPtr(), out_.GetDataSize(), 0);

	// Any error ocurred?
	if (ret < 0) {
		// EWOULDBLOCK?
		if (errno == EAGAIN)
			return 0;
		else
			return -1;
	} else {
		out_.SubDataSize(ret);
		out_.Rearrange();
	}

	return 0;
}

