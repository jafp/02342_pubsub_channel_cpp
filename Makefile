
OBJS 	+= event_loop.o
OBJS 	+= socket.o
OBJS 	+= channel.o
OBJS 	+= main.o 

TARGET 	= pubsub_channel_test 

LDFLAGS = 
CXXFLAGS = -g -std=c++11 -Wall -pedantic

all: $(OBJS) 
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm *.o pubsub_channel_test
