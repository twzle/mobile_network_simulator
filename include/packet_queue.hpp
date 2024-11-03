#pragma once

#include "packet.hpp"

class PacketQueue {
    public:
        PacketQueue(int quant, size_t limit);

        void add_packet(Packet& packet);
        void add_delay(int delay);

        int get_lost_packet_count() const;
        double get_quant() const;
        double get_deficit() const;
        void set_deficit(double deficit);

        Packet front() const;
        int size() const;
        void pop();
        void push(const Packet& packet);

        void print();

    private:
        double quant; // Время выделяемое очереди 
        double deficit; // Накопленный дефицит времени
        size_t limit; // Размер очереди
        int lost_packet_count = 0; // Число потерянных пакетов
        std::priority_queue<Packet, std::vector<Packet>, PacketGreater> packet_queue; // Очередь из обслуживаемых пакетов
};