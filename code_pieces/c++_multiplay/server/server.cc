//
// 2016.04.15 jesssoft
//

#include <signal.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <fcntl.h>
#include <cassert>
#include "../common/socket_data.h"
#include "server.h"

#define MAX_EPOLL_EVENT_COUNT		10000
#define SOCKETKEY_LISTENING_CLIENT	1
#define SOCKETKEY_CLIENT		2

using namespace std;

static bool g_quit_flag = false;

static void 
sig_handler(int signum)
{
	if (SIGINT == signum)
		g_quit_flag = true;
}

Server::Server()
{
	epoll_ = -1;
}

Server::~Server()
{
	Cleanup();
}

bool 
Server::Init(uint16_t port)
{
	if (epoll_ != -1)
		return false;

	// Signals
	signal(SIGPIPE, SIG_IGN);
	signal(SIGINT, sig_handler);

	epoll_ = epoll_create(MAX_EPOLL_EVENT_COUNT);
	if (epoll_ < 0)
		return false;

	if (!StartListening(NULL, port, SOCKETKEY_LISTENING_CLIENT))
		return false;

	srand(time(0));
	return true;
}

bool
Server::Run(void)
{
	assert(epoll_ != -1);

	bool quit = false;
	struct epoll_event ev[MAX_EPOLL_EVENT_COUNT];
	struct tm *t = NULL;
	struct timeval tv_pre;
	struct timeval tv_cur;

	gettimeofday(&tv_pre, NULL);
	while (quit == false) {
		int nready = epoll_wait(epoll_, ev, MAX_EPOLL_EVENT_COUNT, 1);

		// error
		if (nready < 0) {
			if (errno == EINTR)
				;
			else
				g_quit_flag = true;
		} else if (nready == 0) { // timeout
			; // nothing to do
		} else {
			int ret;

			for (int i = 0; i < nready; i++) {
				SocketData *sd = (SocketData *)ev[i].data.ptr;
				assert(sd);
				
				switch (sd->GetSocketKey()) {
				case SOCKETKEY_LISTENING_CLIENT:
					OnAccepted(sd, SOCKETKEY_CLIENT);
					break;
				case SOCKETKEY_CLIENT:
					if (sd->IsShutdown()) {
						OnClosed(sd);
						break;
					}

					ret = sd->Read();
					if (ret < 0) {
						// Error
						OnClosed(sd);
					} else if (ret > 0) {
						// Data to be read
						if (OnRead(sd) == false)
							sd->Shutdown();
					} else { // ret == 0
						// EWOULDBLOCK.
						// Nothing to do
					}
					break;
				default:
					break;
				}
			}
		}

		FlushSockets(SOCKETKEY_CLIENT);

		if (g_quit_flag == true) {
			if (OnCleaning() == true)
				quit = true;
		}

		//
		// Timer
		//
		gettimeofday(&tv_cur, NULL);
		t = localtime(&tv_cur.tv_sec);

		uint32_t diff_ms = 0;
		uint64_t pre_ms;
		uint64_t cur_ms;

		pre_ms = ((uint64_t)tv_pre.tv_sec)*1000 + 
		    tv_pre.tv_usec/1000;
		cur_ms = ((uint64_t)tv_cur.tv_sec)*1000 +
		    tv_cur.tv_usec/1000;
		
		if (cur_ms > pre_ms) {
			diff_ms = (uint32_t)(cur_ms - pre_ms);
		} else {
			diff_ms = 0;
		}

		tv_pre = tv_cur;
		OnEventTimer(t, tv_cur, diff_ms);
	}

	OnCleanup();

	return true;
}

bool 
Server::OnRead(SocketData *sd)
{
	while (true) {
		if (!sd->IsValidMsgComing())
			return false;

		if (!sd->IsMsg())
			break;

		if (!sd->IsValidMsg())
			return false;

		Msg msg;
		sd->GetMsg(&msg);

		if (OnMsg(sd, msg) == false)
			return false;
	}

	sd->Rearrange();
	return true;
}

void 
Server::OnAccepted(SocketData *sd, int socket_key)
{
	assert(sd);

	struct sockaddr_in clt_addr = {0};
	int fd;
	int clt_len;
	int flag = 1;

	clt_len = sizeof(clt_addr);
	fd = accept(sd->GetFd(), (struct sockaddr*)&clt_addr,
	    (socklen_t *)&clt_len);

	// Failed to accept?
	if (fd < 0)
		return;

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		close(fd);
		return;
	}

	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
	    sizeof(flag)) < 0) {
		// Failed
		close(fd);
		return;
	}

	SocketData *new_sd = new SocketData(fd, socket_key);
	new_sd->SetClientName(inet_ntoa(clt_addr.sin_addr));

	if (AddSocketToEpoll(new_sd) == false)
		delete new_sd;
	else
		OnEventAccepted(new_sd, socket_key);
}

void 
Server::OnClosed(SocketData *sd)
{
	assert(sd);

	OnEventClosed(sd);
	DelSocketFromEpoll(sd);
}

void 
Server::FlushSockets(int socket_key)
{
	auto it = map_key_socketdata_.find(socket_key);
	if (it == map_key_socketdata_.end())
		return;

	set_socketdata_t &set_socketdata = it->second;
	for (auto it2 = set_socketdata.begin(); it2 != set_socketdata.end();
	    ++it2) {
		SocketData *sd = *it2;
		assert(sd);

		if (sd->IsShutdown())
			continue;

		if (sd->Flush() != 0)
			sd->Shutdown();
	}
}

bool 
Server::OnCleaning(void)
{
	static int step = 0;

	switch (step) {
	case 0:
		StopListening(SOCKETKEY_LISTENING_CLIENT);
		ShutdownSockets(SOCKETKEY_CLIENT);
		step++;
		break;

	case 1:
		if (GetSocketCount(SOCKETKEY_CLIENT) == 0)
			return true;
		break;
	}
	
	return false;
}

void 
Server::Cleanup(void)
{
	if (epoll_ != -1) {
		close(epoll_);
		epoll_ = -1;
	}
}

void 
Server::StopListening(int socket_key)
{
	auto it = map_key_socketdata_.find(socket_key);
	if (it == map_key_socketdata_.end())
		return;

	set_socketdata_t &set_socketdata = it->second;
	while (!set_socketdata.empty()) {
		auto it2 = set_socketdata.begin();

		SocketData *sd = *it2;
		DelSocketFromEpoll(sd);
	}
}

void 
Server::ShutdownSockets(int socket_key)
{
	auto it = map_key_socketdata_.find(socket_key);
	if (it == map_key_socketdata_.end())
		return;

	set_socketdata_t &set_socketdata = it->second;
	for (auto it2 = set_socketdata.begin(); it2 != set_socketdata.end();
	    ++it2) {
		SocketData *sd = *it2;
		sd->Shutdown();
	}
}

int 
Server::GetSocketCount(int socket_key)
{
	auto it = map_key_socketdata_.find(socket_key);
	if (it == map_key_socketdata_.end())
		return 0;

	set_socketdata_t &set_socketdata = it->second;
	return (int)set_socketdata.size();
}

bool 
Server::AddSocketToEpoll(SocketData *sd)
{
	assert(sd);
	assert(epoll_ != -1);

	struct epoll_event ev = {0};
	ev.events = EPOLLIN;
	ev.data.ptr = sd;
	if (epoll_ctl(epoll_, EPOLL_CTL_ADD, sd->GetFd(), &ev) < 0)
		return false;

	int socket_key = sd->GetSocketKey();

	auto it = map_key_socketdata_.find(socket_key);
	if (it == map_key_socketdata_.end()) {
		set_socketdata_t tmp;
		map_key_socketdata_.insert(
		    map_key_socketdata_t::value_type(socket_key,
			std::move(tmp)));
		it = map_key_socketdata_.find(socket_key);
	}

	assert(it != map_key_socketdata_.end());

	set_socketdata_t &set_socketdata = it->second;
	if (set_socketdata.count(sd) != 0)
		return false;

	set_socketdata.insert(sd);
	return true;
}

void 
Server::DelSocketFromEpoll(SocketData *sd)
{
	assert(sd);
	assert(epoll_ != -1);

	epoll_ctl(epoll_, EPOLL_CTL_DEL, sd->GetFd(), 0);

	int socket_key = sd->GetSocketKey();

	auto it = map_key_socketdata_.find(socket_key);
	if (it == map_key_socketdata_.end())
		return;

	set_socketdata_t &set_socketdata = it->second;
	auto it2 = set_socketdata.find(sd);
	if (it2 == set_socketdata.end())
		return;

	set_socketdata.erase(it2);
	delete sd;
}

SocketData *
Server::StartListening(const char *ip, uint16_t port, int socket_key)
{
	if (epoll_ == -1)
		return NULL;

	int fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0)
		return NULL;

	int sock_opt = 1;
	int ret = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &sock_opt,
	    sizeof(sock_opt));
	if (ret < 0) {
		close(fd);
		return NULL;
	}

	struct sockaddr_in svr_addr = {0};
	svr_addr.sin_family = AF_INET;
	svr_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	svr_addr.sin_port = htons(port);

	if (bind(fd, (struct sockaddr*)&svr_addr, sizeof(svr_addr))) {
		close(fd);
		return NULL;
	}

	if (listen(fd, 5) < 0) {
		close(fd);
		return NULL;
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		close(fd);
		return NULL;
	}

	char buf[1024];
	SocketData *socketData = new SocketData(fd, socket_key);
	socketData->SetClientName(buf);

	if (AddSocketToEpoll(socketData) == false) {
		delete socketData;
		return NULL;
	}

	return socketData;
}

bool
Server::OnMsg(SocketData *sd, Msg &msg)
{
	assert(sd);
	return OnEventMsg(sd, msg);
}

