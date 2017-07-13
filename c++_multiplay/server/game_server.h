//
// 2016.04.15 jesssoft
//
#ifndef __GAME_SERVER_H__
#define __GAME_SERVER_H__

#include <map>
#include <string>
#include <cstdint>
#include "../common/common.h"
#include "server.h"
#include "room.h"
#include "protocol_def.h"
#include "client.h"

// for client.
#define DECLARE_MSG_HANDLER_CLIENT(protocol) \
	bool On##protocol(Client *client, Msg &rcvmsg)
#define DEFINE_MSG_HANDLER_CLIENT(protocol) \
	bool GameServer::On##protocol(Client *client, Msg &rcvmsg)
#define CALL_MSG_HANDLER_CLIENT(protocol) \
	case protocol: ret = On##protocol(client, rcvmsg); break

class GameServer : public Server {
public:
	GameServer(uint32_t timestep);
	virtual ~GameServer();

protected:
	virtual void	OnEventAccepted(SocketData *sd,
			    int socket_key) /*override*/;
	virtual bool	OnEventMsg(SocketData *sd, Msg &rcvmsg) /*override*/;
	virtual void	OnEventClosed(SocketData *sd) /*override*/;
	virtual void	OnEventTimer(struct tm *t, struct timeval &cur_time,
			    uint32_t diff_ms) /*override*/;
	virtual void	OnCleanup(void);

private:
	typedef std::map<uint32_t, Client *> world_t;
	typedef std::map<std::string, Room *> rooms_t;

	uint32_t		acc_id_seed_no_;
	world_t			world_;
	rooms_t			rooms_;
	uint32_t		timestep_;
	uint32_t		nty_elapsed_ms_;
	uint64_t		timestamp_;

	void	CleanupWorld(void);
	void	CleanupRoom(void);
	void	SetTimestamp(Client *client);
	void	SendMsgToWorld(Msg &msg, Client *skip_client = NULL);
	bool	AddToWorld(Client *client);
	void	RemoveFromWorld(Client *client);
	Room	*FindRoom(const std::string &roomkey);
	Room	*CreateRoom(const std::string &roomkey);
	void	DeleteRoom(Room *room);
	bool	AddToRoom(const std::string &roomkey, Client *client);
	void	RemoveFromRoom(Client *client);
	void	SendUpdateToWorld(void);
	void 	SendCS_ENTER_WORLD_ACK(uint32_t ret, Client *client);
	void 	SendCS_ENTER_WORLD_NTY(Client *client, Client *skip_client);
	void	SendCS_LEAVE_WORLD_NTY(Client *client);

	DECLARE_MSG_HANDLER_CLIENT(CS_LOGIN_REQ);
	DECLARE_MSG_HANDLER_CLIENT(CS_ENTER_WORLD_REQ);
	DECLARE_MSG_HANDLER_CLIENT(CS_ENTER_ROOM_REQ);
	DECLARE_MSG_HANDLER_CLIENT(CS_LEAVE_ROOM_REQ);
	DECLARE_MSG_HANDLER_CLIENT(CS_UPDATE_REQ);
};

#endif
