
#ifndef SOCKET_H_
#define SOCKET_H_

#include <string>
#include "socket_file_desc.h"

class Socket : public SocketFileDescriptor {
public:
	Socket();
	virtual ~Socket() {}

	void connect(const std::string& host, const std::string& port);
	void close();
};


#endif
