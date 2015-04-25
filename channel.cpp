
#include "channel.h"
#include <iostream>
#include <sstream>

using namespace std;
using boost::asio::ip::tcp;

namespace jafp {

void Channel::open(std::string host) {
	tcp::resolver resolver(io_service_);
    tcp::resolver::query query(host, "45332");
    tcp::resolver::iterator iterator = resolver.resolve(query);
    boost::asio::connect(socket_, iterator);
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
	boost::asio::async_read(socket_, 
		boost::asio::buffer(header_, 4), [this](boost::system::error_code ec, size_t) {
			if (!ec) {
				readMessageBody(unpack(header_));
			} else {
				cerr << ec << endl;
			}
		}
	);
}

void Channel::readMessageBody(int len) {
	boost::asio::async_read(socket_, 
		boost::asio::buffer(body_, len), [this, len](boost::system::error_code ec, size_t) {
			if (!ec) {
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
			} else {
				cerr << ec << endl;
			}
		}
	);	
}

void Channel::blockingSendControl(std::string type, std::string name, std::string data) {
	std::stringstream str;

	// TODO This seems too simple!
	str << type << ":" << name << ":" << data;
	std::string payload = str.str();

	std::array<char, HeaderLength> header = pack(payload.size());
	boost::asio::write(socket_, boost::asio::buffer(header, HeaderLength));
	boost::asio::write(socket_, boost::asio::buffer(payload));
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

}
