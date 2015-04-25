
#ifndef CHANNEL_H_
#define CHANNEL_H_

#include <functional>
#include <array>
#include <boost/asio.hpp>

namespace jafp {

/*
class Message {
public:
	Message(std::string msg);

private:
	std::string name_;
	std::string data_;
};
*/

class Channel {
public:
	static constexpr int Port = 45332;
	static constexpr int HeaderLength = 4;
	static constexpr int MaxMessageLength = 1024;

	using Callback = std::function<void(std::string name, std::string data)>;

	Channel(boost::asio::io_service& io_service)
		: io_service_(io_service), socket_(io_service) {}
	virtual ~Channel() {}

	Channel(const Channel& ch) = delete;
	Channel& operator=(const Channel& ch) = delete;

	void open(std::string host);
	void close();

	void publish(std::string name, std::string message);
	void subscribe(std::string name, Callback callback);

private:
	void blockingSendControl(std::string type, std::string name, std::string data);
	void readMessageHeader();
	void readMessageBody(int len);
	int unpack(std::array<char, HeaderLength> data);
	std::array<char, HeaderLength> pack(int header);

	boost::asio::io_service& io_service_;
	boost::asio::ip::tcp::socket socket_;
	std::array<char, HeaderLength> header_;
	std::array<char, MaxMessageLength> body_;
	std::map<std::string, Callback> subscriptions_;
};

}

#endif
