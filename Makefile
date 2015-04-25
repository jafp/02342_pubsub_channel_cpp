
all:
	g++ -std=c++11 -c channel.cpp -Wall -Wextra -pedantic
	g++ -std=c++11 -c main.cpp -Wall -Wextra -pedantic
	g++ -std=c++11 channel.o main.o -o pubsub_channel_test -lboost_system -pthread

clean:
	rm *.o pubsub_channel_test
