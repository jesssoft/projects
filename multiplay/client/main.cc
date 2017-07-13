//
// 2016.04.16 jesssoft
//
#include <iostream>
#include <getopt.h>
#include "game.h"

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
	"Usage: %s [-h] [-a ip] [-p port] [-k room-key]\n"
	"  -a           Ip address.\n"
	"  -p           Port number.\n"
	"  -k           Room-key.\n"
	"  -h, --help   Help\n";

	printf(desc, program_name);
}

int 
main(int argc, char *argv[])
{
	int c, opt_idx;
	const char *ip = "127.0.0.1";
	const char *roomkey = "default";
	int port = 2000;

	for (;;) {
		c = getopt_long(argc, argv, "ha:p:k:", long_opts, &opt_idx);
		if (c == -1)
			break;

		switch (c) {
		case 0:
			PrintUsage(argv[0]);
			return 0;
		case 'h':
			PrintUsage(argv[0]);
			return 0;
		case 'a':
			ip = optarg;
			break;
		case 'p':
			port = atoi(optarg);
			break;
		case 'k':
			roomkey = optarg;
			break;
		default:
			PrintUsage(argv[0]);
			return 0;
		}
	}

	Game *game = new Game;

	if (!game->Init(800, 600)) {
		delete game;
		return -1;
	}

	if (!game->Run(ip, port, roomkey)) {
		delete game;
		return -1;
	}

	delete game;
	return 0;
}
