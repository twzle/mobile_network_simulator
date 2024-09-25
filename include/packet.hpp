#include <iostream>
#include <queue>
#include <chrono>
#include <thread>

typedef std::chrono::system_clock SystemClock;
typedef std::chrono::milliseconds ms;
typedef SystemClock::time_point time_point;

class Packet {
    public:
        Packet(int size);
        ms get_size();
        bool get_retry();
        void set_retry();

        time_point get_scheduled_at();
        void set_scheduled_at(time_point scheduled_at);

    private:
        ms size; // Размер пакета (время на обслуживание) в мс
        time_point scheduled_at; // Время в которое для пакета было запланировано обслуживание
        bool retry; // Превысил ли пакет дефицит
};