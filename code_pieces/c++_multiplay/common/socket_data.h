//
// 2016.04.15 jesssoft
//
#ifndef __SOCKET_DATA_H__
#define __SOCKET_DATA_H__

#include <string>
#include "iobuf.h"

#define FD_NULL		-1

class Msg;

class SocketData {
public:
	SocketData(int fd, int socket_key,
	    unsigned int iobuf_size = 32768); //32k
	virtual ~SocketData();

	inline int	GetFd(void)		{ return fd_; };
	inline int	GetSocketKey(void)	{ return socket_key_; };

	bool		IsValidMsgComing(void);
	bool		IsValidMsg(void);
	bool		IsMsg(void);
	bool		GetMsg(Msg *msg, bool peek = false);
	void		Rearrange(void);
	// ret < 0: error(can't read), ret >= 0: sucess.
	int		Read(void);
	void		Close(void);
	inline bool	IsNullFd(void)	{ return GetFd() == FD_NULL; };
	void		Shutdown(void);
	inline bool	IsShutdown(void) { return shutdown_; };
	inline void	SetData(void *data) { data_ = data; };
	inline void	*GetData(void)	{ return data_; };
	unsigned int	GetIncomingDataSize(void);
	unsigned int 	GetOutgoingDataSize(void);
	int		SendMsg(Msg &msg);
	// ret < 0: error(can't send), ret >= 0: sucess.
	int		Flush(void);
	inline const std::string &
			GetClientName(void) { return client_name_; };
	inline void	SetClientName(const char *name) { 
				client_name_ = name; 
			};
	inline std::string &
			GetIp(void) { return ip_; };
	inline void	SetIp(const char *ip) { ip_ = ip; };
	inline unsigned int
			GetIOBufSize(void) { return cIO_BUF_SIZE; };

private:
	const unsigned int cIO_BUF_SIZE;

	int		fd_;
	int		socket_key_;
	IOBuf		in_;
	IOBuf		out_;
	void		*data_;
	bool		shutdown_;
	std::string	client_name_;
	std::string	ip_;
};

#endif
