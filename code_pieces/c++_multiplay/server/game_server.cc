//
// 2016.04.15 jesssoft
//

#include <iostream>
#include "game_server.h"

using namespace std;

GameServer::GameServer(uint32_t timestep)
{ 
	acc_id_seed_no_ = 1; 
	nty_elapsed_ms_ = 0;
	timestep_ = timestep;
	timestamp_ = 0;
}

GameServer::~GameServer()
{
}

void
GameServer::OnEventAccepted(SocketData *sd, int socket_key)
{
	cout << "Accepted" << endl;

	Client *client = new Client(sd);
	sd->SetData(client);
}

bool
GameServer::OnEventMsg(SocketData *sd, Msg &rcvmsg)
{
	Client *client = static_cast<Client*>(sd->GetData());
	unsigned short msg_id = rcvmsg.GetId();

	if (sd->IsShutdown())
		return false;

	bool ret = true;
	switch (msg_id) {
	CALL_MSG_HANDLER_CLIENT(CS_LOGIN_REQ);
	CALL_MSG_HANDLER_CLIENT(CS_ENTER_WORLD_REQ);
	CALL_MSG_HANDLER_CLIENT(CS_ENTER_ROOM_REQ);
	CALL_MSG_HANDLER_CLIENT(CS_LEAVE_ROOM_REQ);
	CALL_MSG_HANDLER_CLIENT(CS_UPDATE_REQ);
	default:
		break;
	}

	return ret;
}

void
GameServer::OnEventClosed(SocketData *sd)
{
	Client *client = static_cast<Client *>(sd->GetData());
	assert(client);

	cout << "Closed: acc_id: " << client->GetAccId() << endl;

	if (world_.count(client->GetAccId())) {
		RemoveFromRoom(client);
		RemoveFromWorld(client);
	}

	sd->SetData(NULL);
	delete client;
}

void
GameServer::OnEventTimer(struct tm *t, struct timeval &cur_time,
    uint32_t diff_ms)
{
	//
	// UPDATE time check
	//
	nty_elapsed_ms_ += diff_ms;
	if (nty_elapsed_ms_ > timestep_) {
		nty_elapsed_ms_ %= timestep_;
		SendUpdateToWorld();
	}

	//
	// Timestamp
	//
	timestamp_ += diff_ms;
}

void
GameServer::OnCleanup(void)
{
	cout << "OnCleanup() called ----" << endl;
	CleanupRoom();
	CleanupWorld();
	cout << "-----------------------" << endl;
}

void
GameServer::CleanupRoom(void)
{
	for (auto it = rooms_.begin(); it != rooms_.end(); ++it) {
		Room *room = it->second;
		cout << "Cleanup room: " << room->GetRoomKey() << endl;
		delete room;
	}
	rooms_.clear();
}

void
GameServer::CleanupWorld(void)
{
	for (auto it = world_.begin(); it != world_.end(); ++it) {
		Client *client = it->second;
		cout << "Cleanup client: " << client->GetAccId() << endl;
		delete client;
	}
	world_.clear();
}

bool
GameServer::AddToWorld(Client *client)
{
	uint32_t acc_id = client->GetAccId();

	if (!world_.insert(world_t::value_type(acc_id, client)).second)
		return false;

	return true;
}

void
GameServer::RemoveFromWorld(Client *client)
{
	uint32_t acc_id = client->GetAccId();

	world_.erase(acc_id);
	SendCS_LEAVE_WORLD_NTY(client);
}

Room *
GameServer::FindRoom(const std::string &roomkey)
{
	auto it = rooms_.find(roomkey);
	if (it == rooms_.end())
		return NULL;

	return it->second;
}

Room *
GameServer::CreateRoom(const std::string &roomkey)
{
	Room *room;

	room = new Room(roomkey);
	if (!rooms_.insert(rooms_t::value_type(roomkey, room)).second) {
		delete room;
		return NULL;
	}

	return room;
}

void
GameServer::DeleteRoom(Room *room)
{
	rooms_.erase(room->GetRoomKey());
	delete room;
}

bool
GameServer::AddToRoom(const std::string &roomkey, Client *client)
{
	Room *room;

	room = FindRoom(roomkey);
	if (!room)
		room = CreateRoom(roomkey);
	
	client->SetRoomKey(roomkey);
	room->AddClient(client);

	room->SendCS_ENTER_ROOM_ACK(CS_ENTER_ROOM_ACK_SUCCESS, client);
	room->SendCS_ENTER_ROOM_NTY(client, client);
	return true;
}

void
GameServer::RemoveFromRoom(Client *client)
{
	Room *room;

	room = client->GetRoom();
	if (!room)
		return;

	room->RemoveClient(client);
	if (room->GetCount() == 0)
		DeleteRoom(room);
	else
		room->SendCS_LEAVE_ROOM_NTY(client);
}

void
GameServer::SendUpdateToWorld(void)
{
	for (auto it = rooms_.begin(); it != rooms_.end(); ++it) {
		Room *room = it->second;
		
		room->SendCS_UPDATE_NTY();
	}
}

void
GameServer::SetTimestamp(Client *client)
{
	client->SetTimestamp(timestamp_);
}

void
GameServer::SendMsgToWorld(Msg &msg, Client *skip_client)
{
	for (auto it = world_.begin(); it != world_.end(); ++it) {
		Client *other = it->second;
		if (other == skip_client)
			continue;

		other->SendMsg(msg);
	}
}

/////////////////////////////////////////////////////////////////////////////	
// Senders 
/////////////////////////////////////////////////////////////////////////////	
void 
GameServer::SendCS_ENTER_WORLD_ACK(uint32_t ret, Client *client)
{
	Msg msg(CS_ENTER_WORLD_ACK);

	msg << ret;
	msg << client->GetAccId();

	int count = (int)world_.size();
	msg << count;

	for (auto it = world_.begin(); it != world_.end(); ++it) {
		Client *other = it->second;
		msg << other->GetAccId();
	}

	client->SendMsg(msg);
}

void 
GameServer::SendCS_ENTER_WORLD_NTY(Client *client, Client *skip_client)
{
	Msg msg(CS_ENTER_WORLD_NTY);

	msg << client->GetAccId();
	SendMsgToWorld(msg, skip_client);
}

void
GameServer::SendCS_LEAVE_WORLD_NTY(Client *client)
{
	Msg msg(CS_LEAVE_WORLD_NTY);

	msg << client->GetAccId();
	SendMsgToWorld(msg, client);
}

/////////////////////////////////////////////////////////////////////////////	
// Handlers
/////////////////////////////////////////////////////////////////////////////	
DEFINE_MSG_HANDLER_CLIENT(CS_LOGIN_REQ)
{
	uint32_t	ver;

	rcvmsg >> ver;

	if (ver != PROTOCOL_VERSION)
		return false;

	cout << "Logged in" << endl;
	client->SetAccId(acc_id_seed_no_++);
	client->SendCS_LOGIN_ACK(CS_LOGIN_ACK_SUCCESS);
	return true;
}

DEFINE_MSG_HANDLER_CLIENT(CS_ENTER_WORLD_REQ)
{
	bool ret = AddToWorld(client);
	if (!ret)
		return false;

	SendCS_ENTER_WORLD_ACK(CS_ENTER_WORLD_ACK_SUCCESS, client);
	SendCS_ENTER_WORLD_NTY(client, client);
	return true;
}

DEFINE_MSG_HANDLER_CLIENT(CS_ENTER_ROOM_REQ)
{
	string roomkey;

	rcvmsg >> roomkey;

	SetTimestamp(client);
	client->SetRoomKey(roomkey);

	bool ret = AddToRoom(roomkey, client);
	if (!ret)
		return false;

	return true;
}

DEFINE_MSG_HANDLER_CLIENT(CS_LEAVE_ROOM_REQ)
{
	Room *room = client->GetRoom();
	if (!room)
		return false;

	RemoveFromRoom(client);
	return true;
}

DEFINE_MSG_HANDLER_CLIENT(CS_UPDATE_REQ)
{
	client->UpdatePosDataFromMsg(rcvmsg);
	client->SendCS_UPDATE_ACK(CS_UPDATE_ACK_SUCCESS);
	return true;
}

