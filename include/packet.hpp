#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include "types.hpp"

class Packet {
    public:
        Packet(int size);
        double get_size() const;
        bool get_retry() const;
        void set_retry();

        double get_scheduled_at() const;
        void set_scheduled_at(double scheduled_at);

    private:
        double size; // Размер пакета (время на обслуживание) в мс
        double scheduled_at; // Время в которое для пакета было запланировано обслуживание
        bool retry; // Превысил ли пакет дефицит
};

struct PacketGreater
{
    bool operator()(const Packet &lhs, const Packet &rhs) const
    {
        return lhs.get_scheduled_at() > rhs.get_scheduled_at();
    }
};