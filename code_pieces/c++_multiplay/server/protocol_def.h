//
// 2016.04.15 jesssoft
//

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

//////////////////////////////////////////////////////////////////////////
//
// DO NOT DELETE the next line '///@ 1'. 
// It's the seed number for proto no. 
//
///@ 1
//
//////////////////////////////////////////////////////////////////////////

#define PROTOCOL_VERSION					1	
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
#define CS_LOGIN_REQ						1
// uint		ver;
#define CS_LOGIN_ACK						2
// uint		ret;
// uint		acc_id;
#define CS_LOGIN_ACK_SUCCESS					0
#define CS_LOGIN_ACK_FAILED					1
//////////////////////////////////////////////////////////////////////////
#define CS_ENTER_WORLD_REQ					3
// no params;
#define CS_ENTER_WORLD_ACK					4
// uint		ret;
// int		count;
// {
// uint		acc_id;
// }
#define CS_ENTER_WORLD_ACK_SUCCESS				0
#define CS_ENTER_WORLD_ACK_FAILED				1

#define CS_ENTER_WORLD_NTY					5
// uint		acc_id;
//////////////////////////////////////////////////////////////////////////
#define CS_LEAVE_WORLD_NTY					6
// uint		acc_id;
//////////////////////////////////////////////////////////////////////////
#define CS_ENTER_ROOM_REQ					7
// string	roomkey;

#define CS_ENTER_ROOM_ACK					8
// uint		ret; 
// -- pos data --
// uint		acc_id;
// uint		seq_no;
// uint64	timestamp;
// int		angle;
// int		x;
// int		y;
// int		step_count;
// -- end --
// int		count;
// {
// -- pos data --
// uint		acc_id;
// uint		seq_no;
// uint64	timestamp;
// int		angle;
// int		x;
// int		y;
// int		step_count;
// -- end --
// }
#define CS_ENTER_ROOM_ACK_SUCCESS				0
#define CS_ENTER_ROOM_ACK_FAILED				1

#define CS_ENTER_ROOM_NTY					9
// -- pos data --
// uint		acc_id;
// uint		seq_no;
// uint64	timestamp;
// int		angle;
// int		x;
// int		y;
// int		step_count;
// -- end --

//////////////////////////////////////////////////////////////////////////
#define CS_LEAVE_ROOM_REQ					10
// not yet

#define CS_LEAVE_ROOM_ACK					11
#define CS_LEAVE_ROOM_ACK_SUCCESS				0
#define CS_LEAVE_ROOM_ACK_FAILED				1
// not yet

#define CS_LEAVE_ROOM_NTY					12
// not yet

//////////////////////////////////////////////////////////////////////////
#define CS_UPDATE_REQ						13
// -- pos data --
// uint		acc_id;
// uint		seq_no;
// uint64	timestamp;
// int		angle;
// int		x;
// int		y;
// int		step_count;
// -- end --
#define CS_UPDATE_ACK						14
// uint		ret;
// -- pos data --
// uint		acc_id;
// uint		seq_no;
// uint64	timestamp;
// int		angle;
// int		x;
// int		y;
// int		step_count;
// -- end --
#define CS_UPDATE_ACK_SUCCESS					0
#define CS_UPDATE_ACK_FAILED					1

#define CS_UPDATE_NTY						15
// uint64	timestamp;
// int		count;
// {
// -- pos data --
// uint		acc_id;
// uint		seq_no;
// uint64	timestamp;
// int		angle;
// int		x;
// int		y;
// int		step_count;
// -- end --
// }
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#endif
