//
// 2016.05.17 jesssoft
//

#include <iostream>
#include "room.h"
#include "protocol_def.h"

using namespace std;

Room::Room(const std::string &roomkey)
{
	roomkey_ = roomkey;
}

Room::~Room()
{
}

bool
Room::AddClient(Client *client)
{
	unsigned int acc_id = client->GetAccId();

	client->SetRoom(this);
	if (!clients_.insert(clients_t::value_type(acc_id, client)).second)
		return false;

	cout << "User(" << acc_id << ") added." << endl;
	return true;
}

void
Room::RemoveClient(Client *client)
{
	unsigned int acc_id = client->GetAccId();

	client->SetRoom(NULL);
	clients_.erase(acc_id);

	cout << "User(" << acc_id << ") removed." << endl;
}

int
Room::GetCount(void)
{
	return clients_.size();
}

const std::string &
Room::GetRoomKey(void)
{
	return roomkey_;
}

void
Room::SendMsgToAll(Msg &msg, Client *skip_client)
{
	for (auto it = clients_.begin(); it != clients_.end(); ++it) {
		Client *other = it->second;
		if (other == skip_client)
			continue;

		other->SendMsg(msg);
	}
}

void
Room::SendCS_ENTER_ROOM_ACK(uint32_t ret, Client *client)
{
	Msg msg(CS_ENTER_ROOM_ACK);

	msg << ret;
	client->AddPosDataToMsg(msg);

	int count = (int)clients_.size();
	msg << count;

	for (auto it = clients_.begin(); it != clients_.end(); ++it) {
		Client *other = it->second;
		other->AddPosDataToMsg(msg);
	}

	client->SendMsg(msg);
}

void
Room::SendCS_ENTER_ROOM_NTY(Client *client, Client *skip_client)
{
	Msg msg(CS_ENTER_ROOM_NTY);

	client->AddPosDataToMsg(msg);
	SendMsgToAll(msg, skip_client);
}

void
Room::SendCS_LEAVE_ROOM_NTY(Client *client)
{
	Msg msg(CS_LEAVE_ROOM_NTY);

	msg << client->GetAccId();
	SendMsgToAll(msg, client);
}

void
Room::SendCS_UPDATE_NTY(void)
{
	Msg msg(CS_UPDATE_NTY);

	int count = (int)clients_.size();
	msg << (uint64_t)0 /* Timestamp */
		<< count;

	for (auto it = clients_.begin(); it != clients_.end(); ++it) {
		Client *other = it->second;

		other->AddPosDataToMsg(msg);
		other->ResetStepCount();
	}

	SendMsgToAll(msg);
}

