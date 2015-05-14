
#ifndef EVENT_LOOP_H_
#define EVENT_LOOP_H_

#include <functional>
#include <memory>
#include <vector>
#include <iostream>
#include <chrono>

class FileDescriptor;

class EventLoop {
public:
	using Callback = std::function<void(size_t)>;
	
	enum class State {
		Started, Stopped
	};

	enum class Type {
		Read, Write
	};

	enum class Action {
		Continue, End
	};

	struct Io {
		int fd;
		char* buf;
		int len;
		Type type;
		Callback cb;

		Io(int fd_, char* buf_, int len_, Type type_, Callback cb_)
			: fd(fd_), buf(buf_), len(len_), type(type_), cb(cb_) {}
	};

	using TimerCallback = std::function<Action(void)>;

	struct Timer {
		TimerCallback callback;
		std::chrono::milliseconds interval;
		std::chrono::high_resolution_clock::time_point last_run;

		Timer(TimerCallback cb, std::chrono::milliseconds intv, 
			std::chrono::high_resolution_clock::time_point lr)
			: callback(cb), interval(intv), last_run(lr) {}
	};

	EventLoop();
	EventLoop(const EventLoop&) = delete;
	EventLoop& operator=(EventLoop&) = delete;

	/**
	 * Run event loop. 
	 */
	void run();

	/**
	 * Graceful shutdown of event loop
	 */
	void shutdown() { state_ = State::Stopped; }

	/**
	 * Repeat the given function at fixed intervals
	 */
	void repeat(std::chrono::milliseconds, TimerCallback);

	/**
	 * Read asynchronically from the given file descriptor.
	 */
	void asyncRead(const FileDescriptor&, char*, size_t, Callback);

	/**
	 * Blocking write to the given file descriptor.
	 */
	int write(const FileDescriptor& fd, char*, size_t);

private:
	State state_;
	std::vector<std::shared_ptr<Io>> ios_;
	std::vector<std::shared_ptr<Timer>> timers_;
};

#endif
