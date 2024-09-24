#include <iostream>
#include <queue>
#include <chrono>
#include <thread>

typedef std::chrono::system_clock SystemClock;
typedef std::chrono::milliseconds ms;

class Packet {
    public:
        Packet(int size);
        ms get_size();

    private:
        ms size; // Размер пакета (время на обслуживание) в мс
};