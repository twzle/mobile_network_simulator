#include <iostream>
#include <queue>
#include <chrono>
#include <thread>

typedef std::chrono::system_clock::time_point TimePoint;
typedef std::chrono::system_clock SystemClock;
typedef std::chrono::milliseconds ms;

class Packet {
    public:
        Packet(int size);
        Packet(int size, TimePoint created_at);

        ms get_size();
        TimePoint get_time();

    private:
        ms size;
        TimePoint created_at;
};