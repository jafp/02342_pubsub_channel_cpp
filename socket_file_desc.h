
#ifndef SOCKET_FILE_DESC_H_
#define SOCKET_FILE_DESC_H_

#include "file_desc.h"

class SocketFileDescriptor : public FileDescriptor {
public:
	SocketFileDescriptor(int fd) : FileDescriptor(fd) {}
};

#endif
