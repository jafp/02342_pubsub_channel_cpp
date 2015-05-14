
#ifndef FILE_DESC_H_
#define FILE_DESC_H_

class FileDescriptor {
public:
	FileDescriptor(int fd) : fd_(fd) {}
	virtual ~FileDescriptor() {}
	int get() const { return fd_; }

protected:
	int fd_;
};

#endif
