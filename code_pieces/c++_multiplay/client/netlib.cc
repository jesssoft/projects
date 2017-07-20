//
// 2016.04.16 jesssoft
//
#include <pthread.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include "../common/socket_data.h"
#include "netlib.h"

void *
NetLib::net_thread(void *arg)
{
	NetLib *obj = static_cast<NetLib*>(arg);
	return obj->NetThread();
}

NetLib::NetLib(void)
{
	sd_ = NULL;
	thread_id_ = 0;
	
	pthread_mutex_init(&sync_, NULL);
}

NetLib::~NetLib()
{
	Close();
	pthread_mutex_destroy(&sync_);
}

int 
NetLib::_Connect(const char *ip, unsigned short port)
{
	struct sockaddr_in svraddr;
	int fd;
	int flag = 1;

	fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (fd < 0)
		return -1;

	memset(&svraddr, 0, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_addr.s_addr = inet_addr(ip);
	svraddr.sin_port = htons(port);

	int ret = connect(fd, (struct sockaddr*)&svraddr, sizeof(svraddr));
	if (ret < 0) {
		close(fd);
		return -1;
	}

	if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) {
		close(fd);
		return -1;
	}

	if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char *)&flag,
	    sizeof(flag)) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

bool 
NetLib::Connect(const char *ip, unsigned short port)
{
	int fd;

	if (sd_)
		return false;

	fd = _Connect(ip, port);
	if (fd < 0)
		return false;

	sd_ = new SocketData(fd, 0);
	if (pthread_create(&thread_id_, NULL, net_thread, (void *)this)) {
		perror("Error: pthread_create failed.");
		Close();
		return false;
	}

	return true;
}

void 
NetLib::Close(void)
{
	pthread_mutex_lock(&sync_);
	if (sd_) {
		delete sd_;
		sd_ = NULL;
	}
	pthread_mutex_unlock(&sync_);

	if (thread_id_) {
		pthread_join(thread_id_, NULL);
		thread_id_ = 0;
	}
}

bool 
NetLib::IsClosed(void)
{
	pthread_mutex_lock(&sync_);
	if (sd_ == NULL) {
		pthread_mutex_unlock(&sync_);
		return true;
	}

	if (sd_->IsNullFd()) {
		pthread_mutex_unlock(&sync_);
		return true;
	}	

	if (sd_->IsShutdown()) {
		pthread_mutex_unlock(&sync_);
		return true;
	}

	pthread_mutex_unlock(&sync_);

	return false;
}

bool 
NetLib::IsConnected(void)
{
	return !IsClosed();
}

bool 
NetLib::GetMsg(Msg &msg)
{
	pthread_mutex_lock(&sync_);
	if (sd_ == NULL || sd_->IsShutdown() || sd_->IsMsg() == false) {
		pthread_mutex_unlock(&sync_);
		return false;
	}

	if (!sd_->IsValidMsg()) {
		sd_->Shutdown();
		pthread_mutex_unlock(&sync_);
		return false;
	}

	if (sd_->GetMsg(&msg)) {
		sd_->Rearrange();
		pthread_mutex_unlock(&sync_);
		return true;
	}

	pthread_mutex_unlock(&sync_);
	return false;
}

bool 
NetLib::SendMsg(Msg &msg)
{
	pthread_mutex_lock(&sync_);
	if (sd_ == NULL || sd_->IsShutdown()) {
		pthread_mutex_unlock(&sync_);
		return false;
	}

	if (sd_->SendMsg(msg) != 0) {
		pthread_mutex_unlock(&sync_);
		return false;
	}

	pthread_mutex_unlock(&sync_);
	return true;
}

bool 
NetLib::Flush(void)
{
	pthread_mutex_lock(&sync_);
	if (sd_ == NULL || sd_->IsShutdown()) {
		pthread_mutex_unlock(&sync_);
		return false;
	}

	if (sd_->Flush() != 0) {
		sd_->Shutdown();
		pthread_mutex_unlock(&sync_);
		return false;
	}

	pthread_mutex_unlock(&sync_);
	return true;
}

void *
NetLib::NetThread(void)
{
	fd_set rfds;
	struct timeval tv;
	int ret;
	bool run = true;

	while (run) {
		FD_ZERO(&rfds);

		pthread_mutex_lock(&sync_);
		int fd = -1;
		if (sd_) {
			fd = sd_->GetFd();
			FD_SET(fd, &rfds);
		} else {
			pthread_mutex_unlock(&sync_);
			run = false;
			continue;
		}
		pthread_mutex_unlock(&sync_);

		tv.tv_sec = 0;
		tv.tv_usec = 1000; // 1 mseconds.
		ret = select(fd+1, &rfds, NULL, NULL, &tv);

		if (ret == -1) {
			perror("Error: ");
			run = false;
			continue;
		} else if (ret) {
			pthread_mutex_lock(&sync_);
			if (!sd_) {
				pthread_mutex_unlock(&sync_);
				run = false;
				continue;
			}

			// data.
			int val = sd_->Read();
			if (val < 0) {
				// Error.
				sd_->Close();
				run = false;
			} else if (val > 0) {
				// data read.
			} else {
				// EWOULDBLOCK;
			}
			pthread_mutex_unlock(&sync_);
		} else {
			// timeout.
		}
		
		if (!Flush())
			run = false;
	}

	return NULL;
}

