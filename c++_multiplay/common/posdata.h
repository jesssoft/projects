//
// 2016.04.17 jesssoft
//
#ifndef __POS_DATA_H__
#define __POS_DATA_H__

class Msg;

class PosData {
public:
	PosData();
	PosData(Msg &arg);
	PosData(const PosData &arg);
	~PosData();

	PosData		&operator=(const PosData &arg);

	unsigned int		acc_id_;
	unsigned int		seq_no_;
	unsigned long long	timestamp_;
	int			angle_;
	int			x_;
	int			y_;
	int			step_count_;
	
	void	SetWithoutAccId(const PosData &arg);
	void	SetWithoutAccIdAndStepCount(const PosData &arg);
	void	SetWithoutAccIdAndStepCountAndSeqNo(const PosData &arg);
	void	SetFromMsg(Msg &msg);
	void	AddToMsg(Msg &msg);
};

#endif
