#pragma once

#include "core/state.hpp"
#include "packet.hpp"


class RelevantPacketQueue
{
public:
    RelevantPacketQueue();

    Packet front() const;
    int size() const;
    void pop();
    void push(const Packet &packet);

    void print();

private:
    std::priority_queue<
        Packet, std::vector<Packet>, 
        PacketGreaterByUserPriorityAndTime> packet_queue; // Очередь из обслуживаемых пакетов
};