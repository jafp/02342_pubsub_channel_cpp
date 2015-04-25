
#include <iostream>
#include <exception>
#include <boost/asio.hpp>
#include "channel.h"

using namespace std;
using namespace jafp;
using boost::asio::ip::tcp;

int main(int argc, char** argv) {

    boost::asio::io_service io_service;
    Channel ch(io_service);

    try {
    	ch.open("localhost");

    	if (argc >= 3) {
    		ch.publish(argv[1], argv[2]);
    	} else {
	    	ch.subscribe("foo", [](std::string name, std::string data) {
	    		cout << name << ", " << data << endl;
	    	});
	    	ch.subscribe("shutdown", [&ch](std::string, std::string) {
	    		cout << "Received shutdown :-(" << endl;
	    		ch.close();
	    	});

	    	io_service.run();
	    }
    	ch.close();

   	} catch (std::exception& e) {
   		cerr << e.what() << endl;
   	}

	return 0;
}
