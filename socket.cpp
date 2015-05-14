
#include "socket.h"
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <iostream>


using namespace std;

Socket::Socket() : SocketFileDescriptor(-1) {
}

void Socket::connect(const string& host, const string& port) {
	struct addrinfo hints, *res;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(host.c_str(), port.c_str(), &hints, &res);
	fd_ = ::socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	if (fd_ < 0) {
		cout << "socket: " << strerror(errno) << endl;
	}

	if (::connect(fd_, res->ai_addr, res->ai_addrlen) < 0) {
		cerr << "connect: " << strerror(errno) << endl;
	}

	//cout << "connected " << fd_ << endl;  
}

void Socket::close() {
	::close(fd_);
}
