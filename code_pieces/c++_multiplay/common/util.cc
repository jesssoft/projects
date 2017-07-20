//
// 2016.04.17 jesssoft
//

#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "util.h"

unsigned long long
Util::GetTicks(void)
{
	struct timeval tv;
	unsigned long long ticks;

	gettimeofday(&tv, NULL);
	ticks = tv.tv_sec * 1000; /* sec to msec */
	ticks += tv.tv_usec/1000; /* usec to msec */

	/* msec */
	return ticks;
}

unsigned long long
Util::GetElapsedTicks(unsigned long long pre_ticks,
    unsigned long long cur_ticks)
{
	if (pre_ticks > cur_ticks)
		return 0;

	return cur_ticks - pre_ticks;
}

void
Util::Daemonlize(void)
{
	pid_t pid, sid;

	pid = fork();
	if (pid < 0)
		exit(EXIT_FAILURE);
	if (pid > 0)
		exit(EXIT_SUCCESS);

	umask(0);
	sid = setsid();
	if (sid < 0)
		exit(EXIT_FAILURE);

	if (!freopen("/dev/null", "r", stdin))
		exit(EXIT_FAILURE);
	if (!freopen("/dev/null", "w", stdout))
		exit(EXIT_FAILURE);
	/*
	if (!freopen("/dev/null", "w", stderr))
		exit(EXIT_FAILURE);
	*/
	if (!freopen("stderr.log", "a", stderr))
		exit(EXIT_FAILURE);
}

