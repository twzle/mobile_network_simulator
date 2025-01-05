#pragma once

#include "core/state.hpp"
#include "core/time_generator.hpp"
#include "packet.hpp"

const double DEFICIT_MAX = 1000;

class PacketQueue
{
public:
    PacketQueue(double quant, size_t limit);

    void add_packet(Packet &packet);
    void add_delay(int delay);

    double get_quant() const;
    double get_deficit() const;
    void set_deficit(double deficit);

    Packet front() const;
    int size() const;
    void pop();
    void push(const Packet &packet);

    void print();

private:
    double quant; // Выделяемое очереди время в ресурсных блоках (RB)
    double deficit; // Накопленный дефицит в ресурсных блоках (RB)
    size_t limit; // Размер очереди
    std::priority_queue<Packet, std::vector<Packet>, PacketGreater> packet_queue; // Очередь из обслуживаемых пакетов
};