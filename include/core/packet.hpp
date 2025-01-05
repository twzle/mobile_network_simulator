#pragma once

#include <iostream>
#include <queue>
#include <thread>

#include "const.hpp"

class Packet
{
public:
    Packet(int size, int queue);

    int get_size() const;
    void set_size(int size);

    int get_queue() const;
    void set_queue(int queue);

    double get_scheduled_at() const;
    void set_scheduled_at(double scheduled_at);

private:
    int size;            // Размер пакета (время на обслуживание) в ресурсных блоках (RB)
    double scheduled_at; // Время в которое для пакета было запланировано обслуживание
    int queue;           // Очередь в которую поставлен пакет
};

struct PacketGreater
{
    bool operator()(const Packet &lhs, const Packet &rhs) const
    {
        return lhs.get_scheduled_at() > rhs.get_scheduled_at();
    }
};