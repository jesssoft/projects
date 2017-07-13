//
// 2016.04.15 jesssoft
//

#include "../server/protocol_def.h"
#include "client.h"

Client::Client(SocketData *sd)
{ 
	sd_ = sd; 
	posdata_.acc_id_ = 0;
	posdata_.seq_no_ = 1;
	posdata_.timestamp_ = 0;
	posdata_.x_ = 100 + rand()%400;
	posdata_.y_ = 100 + rand()%300;
	room_ = NULL;
}

Client::~Client()
{
}

void
Client::SetAccId(unsigned int acc_id)
{ 
	posdata_.acc_id_ = acc_id; 
}

unsigned int
Client::GetAccId(void)
{ 
	return posdata_.acc_id_; 
}

void
Client::SetTimestamp(unsigned long long timestamp)
{	
	posdata_.timestamp_ = timestamp;
}

void
Client::AddPosDataToMsg(Msg &msg)
{
	posdata_.AddToMsg(msg);
}

void
Client::SetPosDataFromMsg(Msg &msg)
{
	PosData posdata;

	posdata.SetFromMsg(msg);
	if (msg.IsError())
		return;

	posdata_.SetWithoutAccId(posdata);
}

void
Client::UpdatePosDataFromMsg(Msg &msg)
{
	PosData posdata;

	posdata.SetFromMsg(msg);
	if (msg.IsError())
		return;

	posdata_.SetWithoutAccIdAndStepCount(posdata);
	posdata_.step_count_++;
}

void
Client::ResetStepCount(void)
{
	posdata_.step_count_ = 0;
}

void
Client::SetRoomKey(const std::string &roomkey)
{
	roomkey_ = roomkey;
}

const std::string &
Client::GetRoomKey(void)
{
	return roomkey_;
}

void
Client::SetRoom(Room *room)
{
	room_ = room;
}

Room *
Client::GetRoom(void)
{
	return room_;
}

void
Client::SendMsg(Msg &msg)
{
	assert(sd_);
	sd_->SendMsg(msg);
}

void
Client::SendCS_LOGIN_ACK(unsigned int ret)
{
	Msg msg(CS_LOGIN_ACK);

	msg << ret 
		<< posdata_.acc_id_;
	SendMsg(msg);
}

void
Client::SendCS_UPDATE_ACK(unsigned int ret)
{
	Msg msg(CS_UPDATE_ACK);

	msg << ret;
	AddPosDataToMsg(msg);
	SendMsg(msg);
}

