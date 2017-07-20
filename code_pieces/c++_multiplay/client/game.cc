//
// 2016.04.16 jesssoft
//

#include <iostream>
#include <csignal>
#include <unistd.h>
#include "game.h"
#include "netlib.h"
#include "player.h"

#define TEX_KEY_SPACE		1
#define TEX_KEY_SHIP		2

using namespace std;

static bool g_quit_flag = false;

static void 
sig_handler(int signum)
{
	if (SIGINT == signum)
		g_quit_flag = true;
}

Game::Game()
{
	win_ = NULL;
	ren_ = NULL;
	init_flag_ = false;
	net_ = NULL;
	me_ = NULL;
	space_ = NULL;
	ship_ = NULL;
	timestamp_ = 0;
	roomkey_ = "default";
	width_ = 0;
	height_ = 0;
	update_ticks_ = 0;
}

Game::~Game()
{
	Cleanup();
}

void
Game::Cleanup(void)
{
	if (init_flag_) {
		//
		// Cleanup textures
		//
		for (auto &it: sprites_)
			delete it.second;
		sprites_.clear();

		if (ren_) {
			SDL_DestroyRenderer(ren_);
			ren_ = NULL;
		}

		if (win_) {
			SDL_DestroyWindow(win_);
			win_ = NULL;
		}

		IMG_Quit();
		SDL_Quit();
		init_flag_ = false;
	}

	//
	// Cleanup network lib
	//
	if (net_) {
		net_->Close();
		net_ = NULL;
	}

	//
	// Cleanup players
	//
	for (auto &it: players_)
		delete it.second;
	players_.clear();
	me_ = NULL;
}

bool
Game::Init(int width, int height)
{
	width_ = width;
	height_ = height;

	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_handler);

	//
	// Starting SDL
        //	
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		cerr << "Error: " << SDL_GetError() << endl;
		return false;
	}

	int flags = IMG_INIT_JPG | IMG_INIT_PNG;
	if (IMG_Init(flags) != flags) {
		SDL_Quit();
		return false;
	}

	init_flag_ = true;

	//
	// Opening a window
	//
	win_ = SDL_CreateWindow("Super Space Ship!", SDL_WINDOWPOS_UNDEFINED, 
	    SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
	if (!win_) {
		Cleanup();
		return false;
	}

	//
	// Creating a Renderer
	//
	ren_ = SDL_CreateRenderer(win_, -1, SDL_RENDERER_ACCELERATED);
	if (!ren_) {
		cerr << "Error: " << SDL_GetError() << endl;
		Cleanup();
		return false;
	}

	//
	// Loading a space image
	//
	if (!(space_ = AddSprite(TEX_KEY_SPACE, "img/space.png"))) {
		Cleanup();
		return false;
	}

	//
	// Loading a ship image
	//
	if (!(ship_ = AddSprite(TEX_KEY_SHIP, "img/ship.png"))) {
		Cleanup();
		return false;
	}

	return true;
}

bool
Game::Run(const char *ip, uint16_t port, const char *roomkey)
{
	if (net_)
		return false;

	net_ = new NetLib;
	roomkey_ = roomkey;

	//
	// Try to connect to the server
	//
	if (net_->Connect(ip, port) == false) {
		cerr << "Error: Connect() failed" << endl;
		return false;
	}

	cout << "Connected." << endl;

	//
	// Try to login.
	//
	SendCS_LOGIN_REQ();
	
	//
	// Main loop
	//
	Msg rcvmsg;
	uint64_t pre_ticks = Util::GetTicks();
	uint64_t cur_ticks = pre_ticks;
	uint64_t elapsed_ticks = 0;
	uint64_t play_ticks = 0;
	uint64_t left_frame_ticks = 0;

	while (net_->IsConnected()) {
		//
		// Network processing
		//
		while (net_->GetMsg(rcvmsg)) {
			OnMsg(rcvmsg);
			rcvmsg.Reinit();
		}

		//
		// Quit?
		//
		if (g_quit_flag)
			net_->Close();

		//
		// Keyboard event processing
		//
		OnKeyboard();

		//
		// Animation processing
		//
		OnProcess(cur_ticks, elapsed_ticks);

		//
		// Draw event
		//
		OnDraw();

		//
		// Calculate the play ticks.
		//
		cur_ticks = Util::GetTicks();
		play_ticks = Util::GetElapsedTicks(pre_ticks, cur_ticks);

		// 200ms = 5fps
		// 100ms = 10fps
		// 50ms = 20fps
		// 33ms = 30fps
		// 17ms = 60fps
		// 8ms = 120fps
		// 4ms = 250fps
		// 2ms = 500fps
		// 1ms = 1000fps
		uint64_t frame_interval_ticks = 2;
		if (play_ticks < frame_interval_ticks) {
			// When the client CPU power is eough high 
			left_frame_ticks = frame_interval_ticks - play_ticks;
			usleep(1000 * left_frame_ticks);

			pre_ticks += play_ticks + left_frame_ticks;
			elapsed_ticks = play_ticks + left_frame_ticks;
		} else {
			// When the client CPU power is low 
			pre_ticks = cur_ticks;
			elapsed_ticks = play_ticks;
		}

		//
		// Timestamp
		//
		timestamp_ += elapsed_ticks;
	}

	net_->Close();
	return true;
}

Sprite *
Game::AddSprite(int tex_key, const char *filename)
{
	if (sprites_.count(tex_key))
		return NULL;

	SDL_Surface *img = IMG_Load(filename);
	if (!img)
		return NULL;

	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren_, img);
	SDL_FreeSurface(img);
	if (!tex)
		return NULL;

	Sprite *sprite = new Sprite(tex);
	sprites_.insert(sprites_t::value_type(tex_key, sprite));
	return sprite;
}

Sprite *
Game::FindSprite(int tex_key)
{
	auto it = sprites_.find(tex_key);
	if (it == sprites_.end())
		return NULL;

	return it->second;
}

int
Game::CalcAngle(int cur_angle, int cur_x, int cur_y, int dst_x, int dst_y)
{
	if (cur_x == dst_x && cur_y == dst_y)
		return cur_angle;

	float org_vec_x = 0;
	float org_vec_y = -1;
	float dir_vec_x = dst_x - cur_x;
	float dir_vec_y = dst_y - cur_y;
	float dir_vec_len = sqrt(dir_vec_x * dir_vec_x + 
				dir_vec_y * dir_vec_y);
	float dir_normvec_x = dir_vec_x/dir_vec_len;
	float dir_normvec_y = dir_vec_y/dir_vec_len;

	float dot = dir_normvec_x * org_vec_x +
		dir_normvec_y * org_vec_y;

	float angle = acos(dot) * 180/M_PI;
	if (dst_x < cur_x)
		angle = 360 - angle;

	/*
	cout << "dot: " << dot << endl;
	cout << "angle: " << angle << endl;
	*/

	return angle;
}

void
Game::OnKeyboard(void)
{
	SDL_Event event;
	PosData posdata;

	while (SDL_PollEvent(&event) && !g_quit_flag) {
		if (event.type == SDL_QUIT) {
			g_quit_flag = true;
			continue;
		}

		if (event.type == SDL_MOUSEBUTTONDOWN) {
			if (!me_)
				continue;

			switch (event.button.button) {
			case SDL_BUTTON_RIGHT:
				break;
			case SDL_BUTTON_LEFT:
				if (me_->IsInAction())
					me_->StopAction();

				cout << "mouse(" << event.motion.x
					<< ", " << event.motion.y
					<< ")" << endl;
			
				posdata = me_->GetPosData();
				posdata.angle_ = CalcAngle(posdata.angle_,
				    posdata.x_, posdata.y_,
				    event.motion.x, event.motion.y);
				posdata.x_ = event.motion.x;
				posdata.y_ = event.motion.y;
				posdata.timestamp_ = timestamp_;
				//posdata.seq_no_++;
				posdata.step_count_ = 1;
				BeginAction(posdata);
				break;
			default:
				break;
			}

			continue;
		} else if (event.type != SDL_KEYDOWN) {
			continue;
		}

		//
		// Quit?
		//
		switch (event.key.keysym.sym) {
		case SDLK_ESCAPE:
		case SDLK_q:
			g_quit_flag = true;
			break;
		}

		//
		// User input?
		//
		if (!me_)
			continue;

		if (me_->IsInAction())
			continue;

		bool action_flag = false;
		switch (event.key.keysym.sym) {
		case SDLK_DOWN:
			posdata = me_->GetPosData();
			posdata.angle_ = 180;
			posdata.y_ += CELL_BLOCK_PIXEL;
			action_flag = true;
			break;
		case SDLK_UP:
			posdata = me_->GetPosData();
			posdata.angle_ = 0;
			posdata.y_ -= CELL_BLOCK_PIXEL;
			action_flag = true;
			break;
		case SDLK_LEFT:
			posdata = me_->GetPosData();
			posdata.angle_ = 270;
			posdata.x_ -= CELL_BLOCK_PIXEL;
			action_flag = true;
			break;
		case SDLK_RIGHT:
			posdata = me_->GetPosData();
			posdata.angle_ = 90;
			posdata.x_ += CELL_BLOCK_PIXEL;
			action_flag = true;
			break;
		}

		if (action_flag) {
			cout << "move to(" << posdata.x_
				<< ", " << posdata.y_
				<< ")" << endl;
			posdata.timestamp_ = timestamp_;
			//posdata.seq_no_++;
			posdata.step_count_ = 1;
			BeginAction(posdata);
		}
	}
}

void
Game::OnDraw(void)
{
	SDL_RenderClear(ren_);
	space_->Draw(ren_, width_/2, height_/2, 0);

	for (auto &it: players_) {
		Player *player = it.second;
		
		ship_->Draw(ren_, player->GetX(), player->GetY(),
		    player->GetAngle());
	}

	SDL_RenderPresent(ren_);
}

void
Game::OnProcess(uint64_t cur_ticks, uint64_t elapsed_ticks)
{
	if (!me_)
		return;

	for (auto &it: players_) {
		Player *player = it.second;
		player->OnProcess(cur_ticks, elapsed_ticks);
	}

	update_ticks_ += elapsed_ticks;
	if (update_ticks_ >= 100) {
		update_ticks_ %= 100;

		//if (me_->IsInAction() && me_->IsRoomFlag()) {
		if (me_->IsSyncNeeded()) {
			me_->IncreaseSeqNo();
			SendUpdateToServer(me_->GetPredictPosData());
		}
	}
}

void
Game::BeginAction(PosData &posdata)
{
	me_->PushAction(posdata);
}

void
Game::SendUpdateToServer(PosData &posdata)
{
	SendCS_UPDATE_REQ(posdata);
}

bool
Game::AddToRoom(Player *player)
{
	uint32_t acc_id = player->GetAccId();

	if (!players_.insert(players_t::value_type(acc_id, player)).second)
		return false;

	player->SetRoomFlag();
	cout << "Added to room - id: " << acc_id << endl;
	return true;
}

Player *
Game::RemoveFromRoom(uint32_t acc_id)
{
	Player *player;

	auto it = players_.find(acc_id);
	if (it == players_.end())
		return NULL;

	player = it->second;
	players_.erase(it);

	return player;
}

Player *
Game::FindInWorld(uint32_t acc_id)
{
	Player *player;

	auto it = players_.find(acc_id);
	if (it == players_.end())
		return NULL;

	player = it->second;
	return player;
}

bool
Game::DeleteFromWorld(uint32_t acc_id)
{
	Player *player;

	player = RemoveFromRoom(acc_id);
	if (!player)
		return false;

	delete player;
	return true;
}

void
Game::OnMsg(Msg &rcvmsg)
{
	uint16_t id;
	
	id = rcvmsg.GetId();
	switch (id) {
	CALL_MSG_HANDLER(CS_LOGIN_ACK);
	CALL_MSG_HANDLER(CS_ENTER_WORLD_ACK);
	CALL_MSG_HANDLER(CS_ENTER_WORLD_NTY);
	CALL_MSG_HANDLER(CS_LEAVE_WORLD_NTY);
	CALL_MSG_HANDLER(CS_ENTER_ROOM_ACK);
	CALL_MSG_HANDLER(CS_ENTER_ROOM_NTY);
	CALL_MSG_HANDLER(CS_LEAVE_ROOM_ACK);
	CALL_MSG_HANDLER(CS_LEAVE_ROOM_NTY);
	CALL_MSG_HANDLER(CS_UPDATE_ACK);
	CALL_MSG_HANDLER(CS_UPDATE_NTY);
	}
}

void
Game::Close(void)
{
	net_->Close();
}

void
Game::SendMsg(Msg &msg)
{
	assert(net_);
	net_->SendMsg(msg);
}

////////////////////////////////////////////////////////////////////////////
// Senders
////////////////////////////////////////////////////////////////////////////
void
Game::SendCS_LOGIN_REQ(void)
{
	Msg msg(CS_LOGIN_REQ);
	msg << PROTOCOL_VERSION;

	SendMsg(msg);
}

void 
Game::SendCS_ENTER_WORLD_REQ(void)
{
	Msg msg(CS_ENTER_WORLD_REQ);

	SendMsg(msg);
}

void 
Game::SendCS_ENTER_ROOM_REQ(void)
{
	Msg msg(CS_ENTER_ROOM_REQ);
	msg << roomkey_;

	SendMsg(msg);
}

void 
Game::SendCS_LEAVE_ROOM_REQ(void)
{
	Msg msg(CS_LEAVE_ROOM_REQ);

	SendMsg(msg);
}

void
Game::SendCS_UPDATE_REQ(PosData &posdata)
{
	assert(me_);

	Msg msg(CS_UPDATE_REQ);

	posdata.AddToMsg(msg);
	SendMsg(msg);
}

////////////////////////////////////////////////////////////////////////////
// Handlers 
////////////////////////////////////////////////////////////////////////////
DEF_MSG_HANDLER(CS_LOGIN_ACK)
{
	uint32_t ret;
	uint32_t acc_id;

	rcvmsg >> ret
		>> acc_id;

	switch (ret) {
	case CS_LOGIN_ACK_SUCCESS:
		cout << "Login success!" << endl;
		SendCS_ENTER_WORLD_REQ();
		break;
	default:
		cout << "Login failed - " << ret << endl;
		break;
	}
}

DEF_MSG_HANDLER(CS_ENTER_WORLD_ACK)
{
	cout << "CS_ENTER_WORLD_ACK" << endl;

	uint32_t ret;
	int count;

	rcvmsg >> ret;

	if (ret != CS_ENTER_WORLD_ACK_SUCCESS) {
		Close();
		return;
	}

	rcvmsg >> count;

	SendCS_ENTER_ROOM_REQ();
}

DEF_MSG_HANDLER(CS_ENTER_WORLD_NTY)
{
	cout << "CS_ENTER_WORLD_NTY" << endl;
}

DEF_MSG_HANDLER(CS_LEAVE_WORLD_NTY)
{
	cout << "CS_LEAVE_WORLD_NTY" << endl;
}

DEF_MSG_HANDLER(CS_ENTER_ROOM_ACK)
{
	cout << "CS_ENTER_ROOM_ACK" << endl;

	uint32_t ret;
	int count;

	rcvmsg >> ret;

	//
	// Me
	//
	me_ = new Player(rcvmsg);
	me_->SetMeFlag();
	AddToRoom(me_);
	timestamp_ = me_->GetPosData().timestamp_;

	//
	// Other players
	//
	rcvmsg >> count;
	for (int i = 0; i < count; i++) {
		Player *player = new Player(rcvmsg);
		AddToRoom(player);
	}
}

DEF_MSG_HANDLER(CS_ENTER_ROOM_NTY)
{
	cout << "CS_ENTER_ROOM_NTY" << endl;
	
	Player *player = new Player(rcvmsg);
	AddToRoom(player);
}

DEF_MSG_HANDLER(CS_LEAVE_ROOM_ACK)
{
	cout << "CS_LEAVE_ROOM_NTY" << endl;

	uint32_t acc_id;

	rcvmsg >> acc_id;

	DeleteFromWorld(acc_id);
}

DEF_MSG_HANDLER(CS_LEAVE_ROOM_NTY)
{
	cout << "CS_LEAVE_ROOM_NTY" << endl;

	uint32_t acc_id;

	rcvmsg >> acc_id;

	DeleteFromWorld(acc_id);
}

DEF_MSG_HANDLER(CS_UPDATE_ACK)
{
	cout << "CS_UPDATE_ACK" << endl;

	uint32_t ret;

	rcvmsg >> ret;

	if (ret != CS_UPDATE_ACK_SUCCESS) {
		Close();
		return;
	}

	me_->UpdateMyPosDataFromMsg(rcvmsg);
}

DEF_MSG_HANDLER(CS_UPDATE_NTY)
{
	int count;
	PosData posdata;

	rcvmsg >> timestamp_
		>> count;

	for (int i = 0; i < count; i++) {
		posdata.SetFromMsg(rcvmsg);

		if (posdata.acc_id_ == me_->GetAccId())
			continue;

		Player *player = FindInWorld(posdata.acc_id_);
		player->PushAction(posdata);
	}

	/*
	cout << "CS_UPDATE_NTY: timestamp: " << timestamp_ << endl;
	cout << "               count: " << count << endl;
	*/
}

