//
// 2016.04.17 jesssoft
//

#include "posdata.h"
#include "msg.h"

PosData::PosData()
{
	acc_id_ = 0;
	seq_no_ = 0;
	timestamp_ = 0;
	angle_ = 0;
	x_ = 0;
	y_ = 0;
	step_count_ = 0;
}

PosData::PosData(Msg &msg)
{
	// -- pos data --
	msg >> acc_id_
		>> seq_no_
		>> timestamp_
		>> angle_
		>> x_
		>> y_
		>> step_count_;
	// -- end --
}

PosData::PosData(const PosData &arg)
{
	acc_id_ = arg.acc_id_;
	seq_no_ = arg.seq_no_;
	timestamp_ = arg.timestamp_;
	angle_ = arg.angle_;
	x_ = arg.x_;
	y_ = arg.y_;
	step_count_ = arg.step_count_;
}

PosData::~PosData()
{
}

PosData &
PosData::operator=(const PosData &arg)
{
	acc_id_ = arg.acc_id_;
	seq_no_ = arg.seq_no_;
	timestamp_ = arg.timestamp_;
	angle_ = arg.angle_;
	x_ = arg.x_;
	y_ = arg.y_;
	step_count_ = arg.step_count_;

	return *this;
}

void
PosData::SetWithoutAccId(const PosData &arg)
{
	//acc_id_ = arg.acc_id_;
	seq_no_ = arg.seq_no_;
	timestamp_ = arg.timestamp_;
	angle_ = arg.angle_;
	x_ = arg.x_;
	y_ = arg.y_;
	step_count_ = arg.step_count_;
}

void
PosData::SetWithoutAccIdAndStepCount(const PosData &arg)
{
	//acc_id_ = arg.acc_id_;
	seq_no_ = arg.seq_no_;
	timestamp_ = arg.timestamp_;
	angle_ = arg.angle_;
	x_ = arg.x_;
	y_ = arg.y_;
	//step_count_ = arg.step_count_;
}

void
PosData::SetWithoutAccIdAndStepCountAndSeqNo(const PosData &arg)
{
	//acc_id_ = arg.acc_id_;
	//seq_no_ = arg.seq_no_;
	timestamp_ = arg.timestamp_;
	angle_ = arg.angle_;
	x_ = arg.x_;
	y_ = arg.y_;
	//step_count_ = arg.step_count_;
}

void
PosData::SetFromMsg(Msg &msg)
{
	// -- pos data --
	msg >> acc_id_
		>> seq_no_
		>> timestamp_
		>> angle_
		>> x_
		>> y_
		>> step_count_;
	// -- end --
}

void
PosData::AddToMsg(Msg &msg)
{
	// -- pos data --
	msg << acc_id_
		<< seq_no_
		<< timestamp_
		<< angle_
		<< x_
		<< y_
		<< step_count_;
	// -- end --
}

