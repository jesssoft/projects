//
// 2016.04.16 jesssoft
//
#ifndef __GAME_H__
#define __GAME_H__

#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "../common/common.h"
#include "../server/protocol_def.h"
#include "sprite.h"

#define DEC_MSG_HANDLER(protocol)	void On##protocol(Msg &rcvmsg)
#define DEF_MSG_HANDLER(protocol)	void Game::On##protocol(Msg &rcvmsg)
#define CALL_MSG_HANDLER(protocol) \
	case protocol: On##protocol(rcvmsg); break

class NetLib;
class Msg;
class Player;

class Game {
public:
	Game();
	~Game();

	bool	Init(int width, int height);
	bool	Run(const char *ip, uint16_t port, const char *roomkey);
	void	Cleanup(void);

private:
	typedef std::map<int, Sprite *> sprites_t;
	typedef std::map<uint32_t, Player *> players_t;

	sprites_t		sprites_;
	players_t		players_;
	bool			init_flag_;
	SDL_Window		*win_;
	SDL_Renderer		*ren_;
	NetLib 			*net_;
	Player			*me_;
	Sprite			*space_;
	Sprite			*ship_;
	uint64_t		timestamp_;
	uint64_t		update_ticks_;
	std::string		roomkey_;
	int			width_;
	int			height_;

	Sprite	*AddSprite(int tex_key, const char *filename);
	Sprite	*FindSprite(int tex_key);
	int	CalcAngle(int cur_angle, int cur_x, int cur_y, int dst_x,
		    int dst_y);

	void	BeginAction(PosData &posdata);
	void	SendUpdateToServer(PosData &posdata);
	bool	AddToRoom(Player *player);
	Player	*RemoveFromRoom(uint32_t acc_id);
	Player	*FindInWorld(uint32_t acc_id);
	bool	DeleteFromWorld(uint32_t acc_id);
	void	OnMsg(Msg &rcvmsg);
	void	OnDraw(void);
	void	OnKeyboard(void);
	void	OnProcess(uint64_t cur_ticks, uint64_t elapsed_ticks);

	void	Close(void);
	void	SendMsg(Msg &msg);
	void	SendCS_LOGIN_REQ(void);
	void	SendCS_ENTER_WORLD_REQ(void);
	void	SendCS_ENTER_ROOM_REQ(void);
	void	SendCS_LEAVE_ROOM_REQ(void);
	void	SendCS_UPDATE_REQ(PosData &posdata);

	DEC_MSG_HANDLER(CS_LOGIN_ACK);
	DEC_MSG_HANDLER(CS_ENTER_WORLD_ACK);
	DEC_MSG_HANDLER(CS_ENTER_WORLD_NTY);
	DEC_MSG_HANDLER(CS_LEAVE_WORLD_NTY);
	DEC_MSG_HANDLER(CS_ENTER_ROOM_ACK);
	DEC_MSG_HANDLER(CS_ENTER_ROOM_NTY);
	DEC_MSG_HANDLER(CS_LEAVE_ROOM_ACK);
	DEC_MSG_HANDLER(CS_LEAVE_ROOM_NTY);
	DEC_MSG_HANDLER(CS_UPDATE_ACK);
	DEC_MSG_HANDLER(CS_UPDATE_NTY);
};

#endif

