
#include <iostream>
#include <exception>
#include "channel.h"
#include "event_loop.h"

using namespace std;
using namespace jafp;

class Foo {
public:
    Foo(EventLoop& ev) : counter_{0}, ev_(ev) {  
        ev_.repeat(chrono::milliseconds{500}, bind(&Foo::update, this));
    }

    EventLoop::Action update();

private:
    int counter_;
    EventLoop& ev_;

};

EventLoop::Action Foo::update() {
    cout << "FOO " << (++counter_) << endl;
    return EventLoop::Action::Continue;
}

int main(int argc, char** argv) {

    EventLoop event_loop;
    Channel ch(event_loop);

    Foo f(event_loop);

    // Dummy repeated timer
    event_loop.repeat(chrono::milliseconds{1000}, []() -> EventLoop::Action {
        return EventLoop::Action::Continue;
    });

    try {
        ch.open("localhost");
        if (argc >= 3) {
            // Three (or more) arguments means publish a message
            cout << "Publish: \"" << argv[1] << "\" to " << argv[2] << endl; 
            ch.publish(argv[1], argv[2]);

        } else {
            // Otherwise, listen to 'foo' and 'shutdown'
            ch.subscribe("foo", [](std::string name, std::string data) {
                cout << "Received message on \"" << name << "\": " << data << endl;
            });

            ch.subscribe("shutdown", [&event_loop, &ch](std::string, std::string) {
                cout << "Received shutdown :-(" << endl;
                ch.close();
                event_loop.shutdown();
            });

            event_loop.run();
        }
        ch.close();

    } catch (std::exception& e) {
        cerr << e.what() << endl;
    }

    return 0;
}
