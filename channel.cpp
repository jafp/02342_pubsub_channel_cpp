
#include "channel.h"
#include <iostream>
#include <sstream>

using namespace std;
using namespace jafp;

void Channel::open(std::string host) {
	// TODO(jp) Error handling
    socket_.connect(host, "45332");
    readMessageHeader();
}

void Channel::close() {
	socket_.close();
}

void Channel::publish(std::string name, std::string message) {
	blockingSendControl("publish", name, message);
}

void Channel::subscribe(std::string name, Callback callback) {
	blockingSendControl("subscribe", name, std::string());
	subscriptions_.insert(std::pair<std::string, Callback>(name, callback));
}

void Channel::readMessageHeader() {
	event_loop_.asyncRead(socket_, header_.data(), HeaderLength, [this](size_t len) {
		readMessageBody(unpack(header_));
	});
}

void Channel::readMessageBody(int len) {
	event_loop_.asyncRead(socket_, body_.data(), len, [this, len](size_t actual_read) {
	
		std::string msg(body_.data(), len);

		// TODO Parse message body somewhere else
		size_t pos1 = msg.find_first_of(":");
		size_t pos2 = msg.find_first_of(":", pos1 + 1);
		std::string name = msg.substr(pos1 + 1, pos2 - pos1 - 1);
		std::string data = msg.substr(pos2 + 1);

		try {
			subscriptions_.at(name)(name, data);
		} catch (std::out_of_range) {}

		readMessageHeader();
	});
}

void Channel::blockingSendControl(std::string type, std::string name, std::string data) {
	std::stringstream str;

	// TODO This seems too simple!
	str << type << ":" << name << ":" << data;
	std::string payload = str.str();
	std::array<char, HeaderLength> header = pack(payload.size());

	event_loop_.write(socket_, header.data(), header.size());
	event_loop_.write(socket_, (char*) payload.c_str(), payload.size());
}

int Channel::unpack(std::array<char, HeaderLength> data) {
	return (int) data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3];
}

std::array<char, Channel::HeaderLength> Channel::pack(int header) {
	std::array<char, HeaderLength> data;
	for (size_t i = 0; i < sizeof(header); ++i) {
   		data[i] = (header >> ((sizeof(header)-1-i)*8));
   	}
	return data;
}
