//
// 2016.04.17 jesssoft
//
#ifndef __UTIL_H__
#define __UTIL_H__

class Util {
public:
	static unsigned long long	GetTicks(void);
	static unsigned long long	GetElapsedTicks(
					    unsigned long long pre_ticks,
					    unsigned long long cur_ticks);
	static void Daemonlize(void);
};

#endif
