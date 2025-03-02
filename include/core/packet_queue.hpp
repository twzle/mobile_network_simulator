#pragma once

#include "core/state.hpp"
#include "core/time_generator.hpp"
#include "packet.hpp"

const double DEFICIT_MAX = 1000000;

class PacketQueue
{
public:
    PacketQueue();
    PacketQueue(double quant, size_t limit);

    void schedule_packet(Packet &packet);

    double get_quant() const;
    size_t get_limit() const;
    double get_deficit() const;
    void set_deficit(double deficit);

    Packet front() const;
    int size() const;
    void pop();
    void push(const Packet &packet);

    void print();

private:
    double quant; // Выделяемое очереди время/частоты в ресурсных блоках (RB)
    double deficit; // Накопленный дефицит в ресурсных блоках (RB)
    size_t limit; // Размер очереди
    std::priority_queue<Packet, std::vector<Packet>, PacketGreaterByTime> packet_queue; // Очередь из обслуживаемых пакетов
};