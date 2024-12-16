#pragma once

#include "packet.hpp"

class PacketQueue {
    public:
        PacketQueue(int quant, size_t limit);

        void add_packet(Packet& packet);
        void add_delay(int delay);

        int get_lost_packet_count() const;
        int get_quant() const;
        int get_deficit() const;
        void set_deficit(int deficit);

        Packet front() const;
        int size() const;
        void pop();
        void push(const Packet& packet);

        void print();

    private:
        int quant; // Выделяемое очереди время в ресурсных блоках (RB)
        int deficit; // Накопленный дефицит в ресурсных блоках (RB)
        size_t limit; // Размер очереди
        int lost_packet_count = 0; // Число потерянных пакетов
        std::priority_queue<Packet, std::vector<Packet>, PacketGreater> packet_queue; // Очередь из обслуживаемых пакетов
};