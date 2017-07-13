//
// 2016.05.17 jesssoft
//
#ifndef __ROOM_H__
#define __ROOM_H__

#include <map>
#include "client.h"

class Room {
public:
	Room(const std::string &roomkey);
	~Room();

	const std::string &
		GetRoomKey(void);
	bool	AddClient(Client *client);
	void	RemoveClient(Client *client);
	int	GetCount(void);
	void	SendCS_ENTER_ROOM_ACK(uint32_t ret, Client *client);
	void	SendCS_ENTER_ROOM_NTY(Client *client, Client *skip_client);
	void	SendCS_LEAVE_ROOM_NTY(Client *client);
	void	SendCS_UPDATE_NTY(void);

private:
	typedef std::map<unsigned int, Client *> clients_t;

	std::string	roomkey_;
	clients_t	clients_;

	void	SendMsgToAll(Msg &msg, Client *skip_client = NULL);
};

#endif

