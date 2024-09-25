#include "packet.hpp"

class PacketQueue {
    public:
        PacketQueue(int quant, size_t limit);

        void add_packet(Packet& packet);
        void add_delay(int delay);

        int get_lost_packet_count() const;
        ms get_quant() const;
        ms get_deficit() const;
        void set_deficit(ms deficit);

        Packet& front();
        int size() const;
        void pop();

    private:
        ms quant; // Время выделяемое очереди 
        ms deficit; // Накопленный дефицит времени
        size_t limit; // Размер очереди
        int lost_packet_count = 0; // Число потерянных пакетов
        std::queue<Packet> packet_queue; // Очередь из обслуживаемых пакетов
};