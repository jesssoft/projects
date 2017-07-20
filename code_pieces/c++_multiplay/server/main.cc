//
// 2016.04.15 jesssoft
//

#include <cstdlib>
#include <iostream>
#include <getopt.h>
#include "game_server.h"

#define REFRESH_MS	100 // 10Hz
#define TIMESTEP_MIN	10
#define TIMESTEP_MAX	1000

using namespace std;

static struct option long_opts[] = {
	{"help", no_argument, 0, 'h'},
	{0, 0, 0, 0}
};

static void
PrintUsage(const char *program_name)
{
	const char *desc;

	desc = 
	"Usage: %s [-h] [-t ms]\n"
	"  -d           Daemonlize this server.\n"
	"  -t ms        Set the server timestep in ms(10 ~ 1000)\n"
	"  -h, --help   Help\n";

	printf(desc, program_name);
}

int
main(int argc, char *argv[])
{
	int c, opt_idx;
	unsigned int timestep = REFRESH_MS;
	int daemon_flag = 0;

	for (;;) {
		c = getopt_long(argc, argv, "hdt:", long_opts, &opt_idx);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			PrintUsage(argv[0]);
			return 0;
		case 'h':
			PrintUsage(argv[0]);
			return 0;
		case 't':
			timestep = atoi(optarg);
			if (timestep < TIMESTEP_MIN)
				timestep = TIMESTEP_MIN;
			if (timestep > TIMESTEP_MAX)
				timestep = 1000;
			break;
		case 'd':
			daemon_flag = 1;
			break;
		default:
			PrintUsage(argv[0]);
			return 0;
		}
	}

	if (daemon_flag)
		Util::Daemonlize();

	GameServer *server = new GameServer(timestep);
	if (!server->Init(2000)) {
		delete server;
		return -1;
	}

	cout << "Get started!" << endl;
	if (!server->Run()) {
		delete server;
		return -1;
	}

	cout << "Bye~" << endl;
	delete server;
	return 0;
}
