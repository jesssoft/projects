//
// 2016.04.15 jesssoft
//
#ifndef __SERVER_H__
#define __SERVER_H__

#include <map>
#include <set>
#include "../common/msg.h"

class SocketData;
class Msg;

class Server {
public:
	Server();
	~Server();

	bool		Init(uint16_t port);
	bool		Run(void);
	void		Cleanup(void);

protected:
	virtual void	OnEventAccepted(SocketData *sd, int socket_key) = 0;
	virtual bool	OnEventMsg(SocketData *sd, Msg &rcv_msg) = 0;
	virtual void	OnEventClosed(SocketData *sd) = 0;
	virtual void	OnEventTimer(struct tm *t, struct timeval &cur_time,
			    uint32_t diff_ms) {};
	virtual void	OnCleanup(void) {};

private:
	//
	// Type definition
	//
	typedef std::set<SocketData *>		set_socketdata_t;
	typedef std::map<int, set_socketdata_t>	map_key_socketdata_t; 

	//
	// Data members 
	//
	int			epoll_;
	map_key_socketdata_t	map_key_socketdata_;

	//
	// Methods
	//
	bool		AddSocketToEpoll(SocketData *sd);
	void		DelSocketFromEpoll(SocketData *sd);
	SocketData 	*StartListening(const char *ip, uint16_t port,
			    int socket_key);
	bool		OnCleaning(void);
	void		StopListening(int socket_key);
	void		ShutdownSockets(int socket_key);
	void		FlushSockets(int socket_key);
	void		OnAccepted(SocketData *sd, int socket_key);
	bool		OnMsg(SocketData *sd, Msg &msg);
	void		OnClosed(SocketData *sd);
	bool		OnRead(SocketData *sd);
	int		GetSocketCount(int socket_key);
};

#endif
