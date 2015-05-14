
#include "event_loop.h"
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "file_desc.h"

using namespace std;

using Ms = std::chrono::milliseconds;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<Clock>;

EventLoop::EventLoop() : state_(State::Stopped) {
}

void EventLoop::run() {
	state_ = State::Started;

	timespec ts;
	fd_set read_fds;
	
	while (state_ == State::Started) {

		// Calculate max sleep time before next timer timeout
		Ms max_sleep{500};

		auto now = Clock::now();
		for (auto timer : timers_) {
			auto diff = chrono::duration_cast<Ms>(now - timer->last_run);

			// No sleep if the timer has never run
			if (timer->last_run == TimePoint(Ms{0})) {
				max_sleep = Ms{0};
			}

			auto time_to_next = timer->interval - diff;
			if (time_to_next < Ms{0}) {
				max_sleep = Ms{0};

			} else if (time_to_next < max_sleep) {
				max_sleep = time_to_next;
			}
		}	

		FD_ZERO(&read_fds);
		int max_fd = 0;
		for (auto io : ios_) {
			if (io->type == Type::Read) {
				FD_SET(io->fd, &read_fds);
				if (io->fd > max_fd) {
					max_fd = io->fd;
				}
			}
		}

		// TODO(jp): Make is possible to sleep for more than a second
		ts.tv_sec = 0;
		ts.tv_nsec = chrono::duration_cast<chrono::nanoseconds>(max_sleep).count();
		
		int ret = pselect(max_fd + 1, &read_fds, nullptr, nullptr, &ts, nullptr);
		if (ret > 0) {
			auto it = ios_.begin();

			vector<shared_ptr<Io>> ready_ios;

			while (it != ios_.end()) {
				auto io = *it;

				if (FD_ISSET(io->fd, &read_fds)) {
					/*size_t bytes_read = */::read(io->fd, io->buf, io->len);
					// TODO(jp) Store bytes_read and pass to callback
					ready_ios.push_back(io);
					it = ios_.erase(it);
				} else {
					++it;
				}
			}
			for (auto io : ready_ios) {
				// TODO(jp) Don't just pass zero
				io->cb(0);
			}
		} else if (ret == -1) {
			// TODO(jp) Handle pselect error
		}

		auto it = timers_.begin();
		while (it != timers_.end()) {
			auto timer = *it;
			auto elapsed = chrono::duration_cast<Ms>(Clock::now() - timer->last_run);
			Action action = Action::Continue;

			if (elapsed >= timer->interval) {
				action = timer->callback();
				timer->last_run = Clock::now();
			}

			if (action == Action::End) {
				it = timers_.erase(it);
			} else {
				it++;
			}
		}
	}
}

void EventLoop::repeat(chrono::milliseconds ms, TimerCallback cb) {
	auto timer = make_shared<Timer>(cb, ms, TimePoint(Ms{0}));
	timers_.push_back(timer);
}

void EventLoop::asyncRead(const FileDescriptor& fd, char* buffer, 
	size_t len, Callback cb) {
	ios_.push_back(make_shared<Io>(fd.get(), buffer, len, Type::Read, cb));
}

int EventLoop::write(const FileDescriptor& fd, char* buffer, size_t len) {
	return ::write(fd.get(), buffer, len);
}
