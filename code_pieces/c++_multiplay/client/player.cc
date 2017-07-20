//
// 2016.04.16 jesssoft
//

#include <iostream>
#include <cmath>
#include "player.h"

using namespace std;

Player::Player(uint32_t acc_id)
{
	posdata_.acc_id_ = acc_id;
	cur_action_ = NULL;
	play_ticks_ = 0;
	action_total_ticks_ = 0;
	delta_angle_ = 0;
	delta_x_ = 0;
	delta_y_ = 0;
	org_angle_ = 0;
	org_x_ = 0;
	org_y_ = 0;
	predict_posdata_ = posdata_;
	sync_flag_ = false;
	me_flag_ = false;
}

Player::Player(Msg &msg)
{
	posdata_.SetFromMsg(msg);
	cur_action_ = NULL;
	play_ticks_ = 0;
	action_total_ticks_ = 0;
	delta_angle_ = 0;
	delta_x_ = 0;
	delta_y_ = 0;
	org_angle_ = 0;
	org_x_ = 0;
	org_y_ = 0;
	predict_posdata_ = posdata_;
	sync_flag_ = false;
	me_flag_ = false;
}

Player::~Player()
{
	for (auto &it : actions_)
		delete it;
	actions_.clear();
}

void
Player::SetPosDataFromMsg(Msg &msg)
{
	PosData posdata(msg);

	posdata_.SetWithoutAccId(posdata);
}

void
Player::SetPosDataFrom(PosData &posdata)
{
	posdata_.SetWithoutAccId(posdata);
}

void
Player::IncreaseSeqNo(void)
{
	predict_posdata_.seq_no_ = posdata_.seq_no_;
	posdata_.seq_no_++;
}

void
Player::UpdateMyPosDataFromMsg(Msg &msg)
{
	PosData svr_posdata(msg);

	if (cur_action_) {
		if (svr_posdata.seq_no_ == posdata_.seq_no_) {
			//
			// Currently is moving!
			//
		} else if (svr_posdata.seq_no_ > posdata_.seq_no_) {
			//
			// Cancel current action!
			//
			StopAction();
			posdata_.SetWithoutAccId(svr_posdata);
			cout << "**** Not possible" << endl;
		}
	} else {
		//
		// Accept server action packet from the server
		//
		if (svr_posdata.seq_no_ == posdata_.seq_no_) {
			posdata_.SetWithoutAccId(svr_posdata);
			cout << "**** Correction" << endl;
		} else if (svr_posdata.seq_no_ > posdata_.seq_no_) {
			cout << "**** Jump - Not possible: " 
				<< svr_posdata.seq_no_
				<< " > "
				<< posdata_.seq_no_
				<< endl;

			posdata_.SetWithoutAccId(svr_posdata);
		}
	}

	//
	// Try to delete old actions
	//
	//DeleteOldActions(posdata_.seq_no_);
}

void
Player::DeleteOldActions(uint32_t cur_seq_no)
{
	for (;;) {
		auto it = actions_.begin();
		if (it == actions_.end())
			break;

		if ((*it)->seq_no_ <= cur_seq_no) {
			delete *it;
			actions_.pop_front();
		} else
			break;
	}
}

void
Player::AddPosDataToMsg(Msg &msg)
{
	posdata_.AddToMsg(msg);
}

bool
Player::IsInAction(void)
{
	if (actions_.size())
		return true;

	return false;
}

bool
Player::IsSyncNeeded(void)
{
	return sync_flag_;
}

void
Player::PushAction(PosData &posdata)
{
	// Didn't move? then let's skip this packet for now.
	if (posdata_.x_ == posdata.x_ && posdata_.y_ == posdata.y_)
		return;

	PosData *act = new PosData;

	*act = posdata;
	actions_.push_back(act);
}

void
Player::StopAction(void)
{
	if (!cur_action_)
		return;

	auto it = actions_.begin();
	if (it == actions_.end())
		return;

	delete *it;
	actions_.pop_front();

	cur_action_ = NULL;
	sync_flag_ = false;
}

void
Player::OnProcess(uint64_t cur_ticks, uint64_t elapsed_ticks)
{
	if (!cur_action_) {
		//
		// Apply old ones immediately
		//
		uint32_t limit_count = 3;

		/* Test: do not interpolate.
		if (!me_flag_)
			limit_count = 0;
			*/

		while (actions_.size() > limit_count) {
			auto it = actions_.begin();

			PosData &posdata = *(*it);
			posdata_.SetWithoutAccId(posdata);
			predict_posdata_ = posdata_;

			cout << "Applied without animation: seq_no:" <<
				posdata.seq_no_ << endl;

			delete *it;
			actions_.pop_front();
		}

		//
		// Try to find the oldest one action
		//
		auto it = actions_.begin();
		if (it == actions_.end())
			return;

		cur_action_ = *it;
		play_ticks_ = 0;
		org_x_ = posdata_.x_;
		org_y_ = posdata_.y_;
		
		//
		// Calculate the values of pixel per ms 
		//
		if (cur_action_->x_ != posdata_.x_) {
			delta_x_ = cur_action_->x_ - posdata_.x_;
		} else
			delta_x_ = 0;

		if (cur_action_->y_ != posdata_.y_) {
			delta_y_ = cur_action_->y_ - posdata_.y_;
		} else
			delta_y_ = 0;

		//
		// No movement? - Same position then do nothing!
		//
		if (!delta_x_ && !delta_y_) {
			// The player didn't move.
			delete *it;
			actions_.pop_front();
			cur_action_ = NULL;
			sync_flag_ = false;
		} else {
			//
			// The player moved so calculate speed factor.
			//
			float length = sqrt(delta_x_ * delta_x_ + 
			    delta_y_ * delta_y_);

			float needed_ticks = length / PIXEL_PER_SECOND;
			action_total_ticks_ = needed_ticks * 1000;
		
			// Normalize
			delta_x_ /= length;
			delta_y_ /= length;

			// Apply speed factor
			delta_x_ *= PIXEL_PER_SECOND/1000;
			delta_y_ *= PIXEL_PER_SECOND/1000;

			//
			// Angle
			//
			posdata_.angle_ = cur_action_->angle_;

			//
			// Display information
			//
			cout << "New action gets started! - seq_no: " 
				<< cur_action_->seq_no_ << ", "
				<< "delta_x: " << delta_x_ << ", "
				<< "delta_y: " << delta_y_ << ", "
				<< "action_total_ticks: "
				<< action_total_ticks_ << ", "
				<< "length: " << length
				<< endl;

			//
			// Predict next position
			//
			predict_posdata_ = posdata_;
			predict_posdata_.x_ = org_x_ + delta_x_ * 100;
			predict_posdata_.y_ = org_y_ + delta_y_ * 100;

			//
			// New action started so sync is needed.
			//
			sync_flag_ = true;
		}
	}

	if (!cur_action_) 
		return;

	//
	// Interpolation (Me and Others)
	//
	play_ticks_ += elapsed_ticks;

	posdata_.x_ = org_x_ + delta_x_ * play_ticks_;
	posdata_.y_ = org_y_ + delta_y_ * play_ticks_;

	float predict_time = play_ticks_ + 100;
	if (predict_time > action_total_ticks_)
		predict_time = action_total_ticks_;

	//
	// Predict next position
	//
	predict_posdata_.x_ = org_x_ + delta_x_ * predict_time;
	predict_posdata_.y_ = org_y_ + delta_y_ * predict_time;
	predict_posdata_.seq_no_ = posdata_.seq_no_;
	//predict_posdata_ = posdata_;

	if (play_ticks_ >= action_total_ticks_) {
		//
		// Play time over
		//
		auto it = actions_.begin();
		if (it == actions_.end()) {
			cout << "Something wrong! - animation!" << endl;
			return;
		}

		//
		// Apply the current action information to position data.
		//
		PosData &posdata = *(*it);
		posdata_.SetWithoutAccIdAndStepCountAndSeqNo(posdata);
		predict_posdata_ = posdata_;

		/*
		cout << "Action stopped - seq_no: " 
			<< posdata.seq_no_ << endl;
			*/

		//
		// Cleanup the current action
		//
		delete *it;
		actions_.pop_front();

		cur_action_ = NULL;
		sync_flag_ = false;

		//
		// Try to process the next action
		//
		float rest_ticks = play_ticks_ - action_total_ticks_;

		// Find the next action
		OnProcess(cur_ticks, elapsed_ticks);

		// process the action of the rest time
		if (rest_ticks > 0)
			OnProcess(cur_ticks, elapsed_ticks);
	}
}

