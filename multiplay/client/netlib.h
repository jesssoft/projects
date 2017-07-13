//
// 2016.04.16 jesssoft
//
#ifndef __NET_H__
#define __NET_H__

#include <pthread.h>
#include "../common/msg.h"

class Msg;
class SocketData;

class NetLib {
public:
	NetLib(void);
	virtual ~NetLib();
	
	bool 	Connect(const char *ip, unsigned short port);
	void 	Close(void);
	bool	IsClosed(void);
	bool	IsConnected(void);
	bool	GetMsg(Msg &msg);
	bool	SendMsg(Msg &msg);

private:
	SocketData	*sd_;
	pthread_t	thread_id_;
	pthread_mutex_t	sync_;

	bool	Flush(void);
	void	*NetThread(void);
	int 	_Connect(const char *ip, unsigned short port);
	
	static void *net_thread(void *arg);
};

#endif

