//
// 2016.04.15 jesssoft
//

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "../common/common.h"

class Room;

class Client {
public:
	Client(SocketData *sd);
	~Client();

	void		SetAccId(unsigned int acc_id);
	unsigned int 	GetAccId(void);
	void		SetTimestamp(unsigned long long timestamp);
	void		AddPosDataToMsg(Msg &msg);
	void		SetPosDataFromMsg(Msg &msg);
	void		UpdatePosDataFromMsg(Msg &msg);
	void		ResetStepCount(void);
	void		SetRoomKey(const std::string &roomkey);
	const std::string &
			GetRoomKey(void);
	void		SetRoom(Room *room);
	Room		*GetRoom(void);
	void		SendMsg(Msg &msg);
	void		SendCS_LOGIN_ACK(unsigned int ret);
	void		SendCS_UPDATE_ACK(unsigned int ret);
private:
	SocketData	*sd_;
	PosData		posdata_;
	std::string	roomkey_;
	Room		*room_;
};

#endif

