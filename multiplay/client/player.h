//
// 2016.04.16 jesssoft
//
#ifndef __PLAYER_H__
#define __PLAYER_H__

#include <list>
#include "../common/common.h"

//
// Speed factors
//
#define CELL_BLOCK_PIXEL	20
#define PIXEL_PER_SECOND	300.0f

class Player {
public:
	Player(uint32_t acc_id);
	Player(Msg &msg);
	~Player();

	inline uint32_t		GetAccId(void) { return posdata_.acc_id_; };
	inline void		SetX(int x) { posdata_.x_ = x; };
	inline void		SetY(int y) { posdata_.y_ = y; };
	inline void		AddX(int x) { posdata_.x_ += x; };
	inline void		AddY(int y) { posdata_.y_ += y; };
	inline int		GetX(void) { return posdata_.x_; };
	inline int		GetY(void) { return posdata_.y_; };
	inline int		GetAngle(void) { return posdata_.angle_; };
	inline void		IncSeqNo(void) { posdata_.seq_no_++; };
	inline uint32_t		GetSeqNo(void) { return posdata_.seq_no_; };
	inline PosData		&GetPosData(void) { return posdata_; };
	inline PosData		&GetPredictPosData(void) { 
		return predict_posdata_; 
	};
	inline void		SetMeFlag(void) { me_flag_ = true; };
	void			SetPosDataFromMsg(Msg &msg);
	void			SetPosDataFrom(PosData &posdata);
	void			UpdateMyPosDataFromMsg(Msg &msg);
	void			AddPosDataToMsg(Msg &msg);
	bool			IsInAction(void);
	void			PushAction(PosData &posdata);
	void			StopAction(void);
	void			DeleteOldActions(uint32_t cur_seq_no);
	void			SetActionInterval(uint64_t ticks);
	void			SetRoomFlag(void) { room_flag_ = true; };
	bool			IsRoomFlag(void) { return room_flag_; };
	bool			IsSyncNeeded(void);
	void			IncreaseSeqNo(void);

	void			OnProcess(uint64_t cur_ticks,
				    uint64_t elapsed_ticks);
private:
	typedef std::list<PosData *>	actions_t;

	actions_t		actions_;
	PosData			predict_posdata_;
	PosData			posdata_;
	PosData			*cur_action_;
	uint64_t		play_ticks_;
	uint64_t		action_total_ticks_;
	float			delta_angle_;
	float			delta_x_;
	float			delta_y_;
	int			org_angle_;
	int			org_x_;
	int			org_y_;
	bool			room_flag_;
	bool			sync_flag_;
	bool			me_flag_;
};

#endif
