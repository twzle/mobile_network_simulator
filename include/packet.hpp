#include <iostream>
#include <queue>
#include <thread>
#include "types.hpp"

class Packet {
    public:
        Packet(int size);
        ms get_size() const;
        bool get_retry() const;
        void set_retry();

        time_point get_scheduled_at() const;
        void set_scheduled_at(time_point scheduled_at);

    private:
        ms size; // Размер пакета (время на обслуживание) в мс
        time_point scheduled_at; // Время в которое для пакета было запланировано обслуживание
        bool retry; // Превысил ли пакет дефицит
};

struct PacketGreater
{
    bool operator()(const Packet &lhs, const Packet &rhs) const
    {
        return lhs.get_scheduled_at() > rhs.get_scheduled_at();
    }
};